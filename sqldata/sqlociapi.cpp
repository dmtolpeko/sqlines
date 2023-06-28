/** 
 * Copyright (c) 2016 SQLines
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// SqlOciApi Oracle OCI API

#include <stdio.h>
#include "sqlociapi.h"
#include "str.h"
#include "os.h"

// ODBC, Sybase CT-Lib, MySQL C definitions
#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h> 
#include <ctpublic.h>
#include <mysql.h>

// Constructor
SqlOciApi::SqlOciApi()
{
	_oci_dll = NULL;

	_envhp = NULL;
	_srvhp = NULL;
	_authp = NULL;
	_svchp = NULL;
	_errhp = NULL;

	_stmtp_cursor = NULL;
	_stmtp_insert = NULL;

	_charset_id = 0;

	_ociArrayDescriptorAlloc = NULL;
	_ociAttrGet = NULL;
	_ociAttrSet = NULL;
	_ociBindByPos = NULL;
	_ociDescriptorAlloc = NULL;
	_ociDescriptorFree = NULL;
	_ociDefineByPos = NULL;
	_ociEnvCreate = NULL;
	_ociErrorGet = NULL;
	_ociStmtFetch2 = NULL;
	_ociHandleAlloc = NULL;
	_ociLobCharSetForm = NULL;
	_ociLobCopy = NULL;
	_ociLobCreateTemporary = NULL;
	_ociLobFreeTemporary = NULL;
	_ociLobGetLength2 = NULL;
	_ociLobRead2 = NULL;
	_ociLobTrim = NULL;
	_ociLobWrite = NULL;
	_ociLobWriteAppend = NULL;
	_ociParamGet = NULL;
	_ociServerAttach = NULL;
	_ociServerDetach = NULL;
	_ociSessionBegin = NULL;
	_ociSessionEnd = NULL;
	_ociStmtExecute = NULL;
	_ociStmtPrepare = NULL;
	_ociTransCommit = NULL;

	_bind_long_inplace = 0;
}

// Initialize API
int SqlOciApi::Init()
{
	TRACE("OCI Init() Entered");

	const char *oci_lib_param = (_parameters != NULL) ? _parameters->Get("-oci_lib") : NULL;
	const char *oci_lib = (oci_lib_param != NULL)? oci_lib_param : OCI_DLL; 
	const char *nls_lang = NULL;
	
	if(_parameters != NULL)
		nls_lang = _parameters->Get("-oracle_nls_lang");

#if defined(WIN32) || defined(WIN64)

	if(nls_lang != NULL)
		_putenv((std::string("NLS_LANG=") + nls_lang).c_str()); 
	else
	// Force UTF-8 codepage at the client side if the target loader supports it
	if(_target_api_provider != NULL && _target_api_provider->IsTargetUtf8LoadSupported())
		_putenv("NLS_LANG=American_America.AL32UTF8");

	_oci_dll = LoadLibraryEx(oci_lib, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);

	// DLL load failed
	if(_oci_dll == NULL)
	{
		// Set error for the current search item 
		Os::GetLastErrorText(OCI_DLL_LOAD_ERROR, _native_error_text, 1024);

		// No error set if DLL is 64-bit and current sqldata build is 32-bit
		bool sf = Os::Is64Bit(oci_lib);

		if(sf == true)
			strcpy(_native_error_text, OCI_64BIT_DLL_LOAD_ERROR);

		// Try to find Oracle installation paths if the library is not set explicitly
		if(oci_lib_param == NULL)
			FindOraclePaths();

		for(std::list<std::string>::iterator i = _driver_paths.begin(); i != _driver_paths.end() ; i++)
		{
			std::string loc = (*i);
			loc += OCI_DLL;

			// Try to open DLL
			_oci_dll = LoadLibraryEx(loc.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);

			if(_oci_dll != NULL)
				break;

			// Set error for the current search item 
			Os::GetLastErrorText(OCI_DLL_LOAD_ERROR, _native_error_text, 1024);

			// No error set if DLL is 64-bit
			sf = Os::Is64Bit(OCI_DLL);

			if(sf == true)
				strcpy(_native_error_text, OCI_64BIT_DLL_LOAD_ERROR);
		}
	}
#else
	_oci_dll = Os::LoadLibrary(oci_lib);
#endif

	if(_oci_dll == NULL)
	{
		char *error = Os::LoadLibraryError();
		if(error != NULL)
			strcpy(_native_error_text, error);

		TRACE("OCI Init() OCI load failed");
		TRACE("OCI Init() Left");
		return -1;
	}

	TRACE("OCI Init() OCI successfully loaded");

	// Save the full path of the loaded driver
	Os::GetModuleFileName(_oci_dll, _loaded_driver, 1024);

	// Get function addresses
	_ociArrayDescriptorAlloc = (OCIArrayDescriptorAllocFunc)Os::GetProcAddress(_oci_dll, "OCIArrayDescriptorAlloc");
	_ociAttrGet = (OCIAttrGetFunc)Os::GetProcAddress(_oci_dll, "OCIAttrGet");
	_ociAttrSet = (OCIAttrSetFunc)Os::GetProcAddress(_oci_dll, "OCIAttrSet");
	_ociBindByPos = (OCIBindByPosFunc)Os::GetProcAddress(_oci_dll, "OCIBindByPos");
	_ociDescriptorAlloc = (OCIDescriptorAllocFunc)Os::GetProcAddress(_oci_dll, "OCIDescriptorAlloc");
	_ociDescriptorFree = (OCIDescriptorFreeFunc)Os::GetProcAddress(_oci_dll, "OCIDescriptorFree");
	_ociDefineByPos = (OCIDefineByPosFunc)Os::GetProcAddress(_oci_dll, "OCIDefineByPos");
	_ociEnvCreate = (OCIEnvCreateFunc)Os::GetProcAddress(_oci_dll, "OCIEnvCreate");
	_ociErrorGet = (OCIErrorGetFunc)Os::GetProcAddress(_oci_dll, "OCIErrorGet");
	_ociStmtFetch2 = (OCIStmtFetch2Func)Os::GetProcAddress(_oci_dll, "OCIStmtFetch2");
	_ociHandleAlloc = (OCIHandleAllocFunc)Os::GetProcAddress(_oci_dll, "OCIHandleAlloc");
	_ociHandleFree = (OCIHandleFreeFunc)Os::GetProcAddress(_oci_dll, "OCIHandleFree");
	_ociLobCharSetForm = (OCILobCharSetFormFunc)Os::GetProcAddress(_oci_dll, "OCILobCharSetForm");
	_ociLobCopy = (OCILobCopyFunc)Os::GetProcAddress(_oci_dll, "OCILobCopy");
	_ociLobCreateTemporary = (OCILobCreateTemporaryFunc)Os::GetProcAddress(_oci_dll, "OCILobCreateTemporary");
	_ociLobFreeTemporary = (OCILobFreeTemporaryFunc)Os::GetProcAddress(_oci_dll, "OCILobFreeTemporary");
	_ociLobGetLength2 = (OCILobGetLength2Func)Os::GetProcAddress(_oci_dll, "OCILobGetLength2");
	_ociLobRead2 = (OCILobRead2Func)Os::GetProcAddress(_oci_dll, "OCILobRead2");
	_ociLobTrim = (OCILobTrimFunc)Os::GetProcAddress(_oci_dll, "OCILobTrim");
	_ociLobWrite = (OCILobWriteFunc)Os::GetProcAddress(_oci_dll, "OCILobWrite");
	_ociLobWriteAppend = (OCILobWriteAppendFunc)Os::GetProcAddress(_oci_dll, "OCILobWriteAppend");
	_ociParamGet = (OCIParamGetFunc)Os::GetProcAddress(_oci_dll, "OCIParamGet");
	_ociServerAttach = (OCIServerAttachFunc)Os::GetProcAddress(_oci_dll, "OCIServerAttach");
	_ociServerDetach = (OCIServerDetachFunc)Os::GetProcAddress(_oci_dll, "OCIServerDetach");
	_ociSessionBegin = (OCISessionBeginFunc)Os::GetProcAddress(_oci_dll, "OCISessionBegin");
	_ociSessionEnd = (OCISessionEndFunc)Os::GetProcAddress(_oci_dll, "OCISessionEnd");
	_ociStmtExecute = (OCIStmtExecuteFunc)Os::GetProcAddress(_oci_dll, "OCIStmtExecute");
	_ociStmtPrepare = (OCIStmtPrepareFunc)Os::GetProcAddress(_oci_dll, "OCIStmtPrepare");
	_ociTransCommit = (OCITransCommitFunc)Os::GetProcAddress(_oci_dll, "OCITransCommit");

	// OCIArrayDescriptorAlloc function is not available in 10g Client (do not check it)
	if(_ociAttrGet == NULL || _ociAttrSet == NULL ||
		_ociAttrSet == NULL || _ociBindByPos == NULL || _ociDefineByPos == NULL || 
		_ociDescriptorAlloc == NULL || _ociDescriptorFree == NULL || _ociEnvCreate == NULL ||
		_ociErrorGet == NULL || _ociHandleAlloc == NULL || _ociHandleFree == NULL || _ociLobCharSetForm == NULL ||
		_ociLobCopy == NULL || _ociLobCreateTemporary == NULL || _ociLobFreeTemporary == NULL ||
		_ociLobTrim == NULL || _ociLobWrite == NULL || _ociLobWriteAppend == NULL || _ociParamGet == NULL || 
		_ociSessionBegin == NULL || _ociSessionEnd == NULL || _ociServerAttach == NULL || 
		_ociServerDetach == NULL || _ociStmtPrepare == NULL || _ociStmtExecute == NULL || 
		_ociStmtFetch2 == NULL || _ociTransCommit == NULL)
	{
		strcpy(_native_error_text, "OCI function(s) not found");
		TRACE("OCI Init() Function not found");
		TRACE("OCI Init() Left");
		return -1;
	}

	ReadReservedWords();

	TRACE("OCI Init() Left");
	return 0;
}

// Set the connection string in the API object
void SqlOciApi::SetConnectionString(const char *conn)
{
	SplitConnectionString(conn, _user, _pwd, _db);
}

// Connect to the database
int SqlOciApi::Connect(size_t *time_spent)
{
	if(_connected == true)
		return 0;

	size_t start = (time_spent != NULL) ? Os::GetTickCount() : 0;

	sword rc = _ociEnvCreate((OCIEnv**)&_envhp, (ub4)OCI_THREADED, NULL, NULL, NULL, NULL, 0, NULL);

	if(rc < 0)
		return rc;

	// Allocate handles
	rc = _ociHandleAlloc((dvoid*)_envhp, (dvoid**)&_errhp, OCI_HTYPE_ERROR, 0, NULL);

	if(rc < 0)
		return rc;

	rc = _ociHandleAlloc((dvoid*)_envhp, (dvoid**)&_srvhp, OCI_HTYPE_SERVER, 0, NULL);

	if(rc < 0)
		return rc;

	rc = _ociHandleAlloc((dvoid*)_envhp, (dvoid**)&_svchp, OCI_HTYPE_SVCCTX, 0, NULL);

	if(rc < 0)
		return rc;
                   
	rc = _ociServerAttach(_srvhp, _errhp, (text*)_db.c_str(), (sb4)_db.length(), 0);

	if(rc < 0)
	{
		SetError();

		if(time_spent != NULL)
			*time_spent = Os::GetTickCount() - start;

		return rc;
	}

	rc = _ociAttrSet(_svchp, OCI_HTYPE_SVCCTX, _srvhp, 0, OCI_ATTR_SERVER, _errhp);

	if(rc < 0)
		return rc;

	rc = _ociHandleAlloc(_envhp, (dvoid **)&_authp, OCI_HTYPE_SESSION, 0, NULL);

	if(rc < 0)
		return rc;

	// Set login information
	rc = _ociAttrSet(_authp, OCI_HTYPE_SESSION, (void*)_user.c_str(), (ub4)_user.length(), OCI_ATTR_USERNAME, _errhp);
	rc = _ociAttrSet(_authp, OCI_HTYPE_SESSION, (void*)_pwd.c_str(), (ub4)_pwd.length(), OCI_ATTR_PASSWORD, _errhp);

	rc = _ociSessionBegin(_svchp, _errhp, _authp, OCI_CRED_RDBMS, OCI_DEFAULT);

	// SessionBegin can return OCI_SUCCESS_WITH_INFO (code 1) when for example, connected
	// with the following warning ORA-28002: the password will expire within 7 days
	if(rc < 0)
	{
		SetError();

		if(time_spent != NULL)
			*time_spent = Os::GetTickCount() - start;

		return rc;
	}

	_connected = true;

	// Get the character set ID (873 for AL32UTF8)
	rc = _ociAttrGet(_envhp, OCI_HTYPE_ENV, &_charset_id, 0, OCI_ATTR_ENV_CHARSET_ID, _errhp);

	rc = _ociAttrSet(_svchp, OCI_HTYPE_SVCCTX, _authp, 0, OCI_ATTR_SESSION, _errhp);

    // Initialize session
	InitSession();

	// Set session attributes
	rc = SetSession();

	// Set version of the connected database
	SetVersion();
	
	if(time_spent != NULL)
		*time_spent = Os::GetTickCount() - start;
	
	return rc;
}

// Initialize session
int SqlOciApi::InitSession()
{
	SqlApiBase::InitSession();

	const char *value = NULL;

	if(_parameters != NULL)
		value = _parameters->Get("-fetch_lob_as_varchar");

	if(value != NULL && *value != '\x0' && 
		(_stricmp(value, "yes") == 0 || _stricmp(value, "true") == 0))
		_cursor_fetch_lob_as_varchar = true;

	return 0;
}

// Set session attributes
int SqlOciApi::SetSession()
{
	// Change timestamp format FF means no fraction, FF6 means 6 digits
	const char *query = "ALTER SESSION SET NLS_TIMESTAMP_FORMAT = 'YYYY-MM-DD HH24:MI:SS.FF6'";
	int rc = ExecuteNonQuery(query, NULL);

	query = "ALTER SESSION SET NLS_TIMESTAMP_TZ_FORMAT = 'YYYY-MM-DD HH24:MI:SS.FF6 TZH:TZM'";
	rc = ExecuteNonQuery(query, NULL);

	query = "ALTER SESSION SET NLS_DATE_FORMAT = 'YYYY-MM-DD HH24:MI:SS'";
	rc = ExecuteNonQuery(query, NULL);

	// Force to use period (.) as decimal separator, by default it depends on locale and , 
	// for many European and Latin America countries
	query = "ALTER SESSION SET NLS_NUMERIC_CHARACTERS = '. '";
	rc = ExecuteNonQuery(query, NULL);

	return rc;
}

// Disconnect from the database
void SqlOciApi::Disconnect()
{
	if(_connected == false)
		return;

	int rc = _ociSessionEnd(_svchp, _errhp, _authp, OCI_DEFAULT); 

	rc = _ociServerDetach(_srvhp, _errhp, OCI_DEFAULT);

	_authp = NULL;
	_srvhp = NULL;

	_connected = false;
}

// Deallocate the driver
void SqlOciApi::Deallocate()
{
	Disconnect();
}

// Get row count for the specified object
int SqlOciApi::GetRowCount(const char *object, int *count, size_t *time_spent)
{
	if(object == NULL)
		return -1;

	std::string query = "SELECT COUNT(*) FROM ";
	query += object;

	// Execute the query
	int rc = ExecuteScalar(query.c_str(), count, time_spent);

	return rc;
}

// Execute the statement and get scalar result
int SqlOciApi::ExecuteScalar(const char *query, int *result, size_t *time_spent)
{
	if(query == NULL || _ociHandleAlloc == NULL || _ociStmtPrepare == NULL || _ociDefineByPos == NULL ||
		_ociStmtExecute == NULL || _ociHandleFree == NULL)
		return -1;

	size_t start = GetTickCount();

	OCIStmt *stmtp;
	OCIDefine *defnpp;
	int q_result = 0;

	// Allocate a statement handle
	int rc = _ociHandleAlloc((dvoid*)_envhp, (dvoid**)&stmtp, OCI_HTYPE_STMT, 0, NULL);

	rc = _ociStmtPrepare(stmtp, _errhp, (text*)query, (ub4)strlen(query), OCI_NTV_SYNTAX, OCI_DEFAULT);

	rc = _ociDefineByPos(stmtp, &defnpp, _errhp, 1, &q_result, sizeof(int), SQLT_INT, NULL, NULL, NULL, OCI_DEFAULT);

	// Execute the statement
	rc = _ociStmtExecute(_svchp, stmtp, _errhp, 1, 0, NULL, NULL, OCI_EXACT_FETCH);

	if(rc != 0)
		SetError();

	// Set the query result
	if(result)
		*result = q_result;

	_ociHandleFree(stmtp, OCI_HTYPE_STMT);

	if(time_spent != NULL)
		*time_spent = GetTickCount() - start;

	return rc;
}

// Execute the statement
int SqlOciApi::ExecuteNonQuery(const char *query, size_t *time_spent)
{
	if(query == NULL || _ociHandleAlloc == NULL || _ociStmtPrepare == NULL || _ociStmtExecute == NULL || 
		_ociHandleFree == NULL)
		return -1;

	OCIStmt *stmtp;

	size_t start = GetTickCount();

	int rc = _ociHandleAlloc((dvoid*)_envhp, (dvoid**)&stmtp, OCI_HTYPE_STMT, 0, NULL);

	rc = _ociStmtPrepare(stmtp, _errhp, (text*)query, (ub4)strlen(query), OCI_NTV_SYNTAX, OCI_DEFAULT);

	// Execute the statement
	rc = _ociStmtExecute(_svchp, stmtp, _errhp, 1, 0, NULL, NULL, OCI_DEFAULT);

	if(rc != 0)
		SetError();

	_ociHandleFree(stmtp, OCI_HTYPE_STMT);

	if(time_spent != NULL)
		*time_spent = GetTickCount() - start;

	return rc;
}

// Open cursor and allocate buffers
int SqlOciApi::OpenCursor(const char *query, size_t buffer_rows, int buffer_memory, size_t *col_count, size_t *allocated_array_rows, 
		int *rows_fetched, SqlCol **cols, size_t *time_spent, bool catalog_query, std::list<SqlDataTypeMap> * /*dtmap*/)
{
	if(query == NULL)
		return -1;

	TRACE("OCI OpenCursor() Entered");
	size_t start = GetTickCount();

	// Allocate statement handle
	int rc = _ociHandleAlloc((dvoid*)_envhp, (dvoid**)&_stmtp_cursor, OCI_HTYPE_STMT, 0, NULL);

	// Prepare the query
	rc = _ociStmtPrepare(_stmtp_cursor, _errhp, (text*)query, (ub4)strlen(query), OCI_NTV_SYNTAX, OCI_DEFAULT);

	// Execute the query
	rc = _ociStmtExecute(_svchp, _stmtp_cursor, _errhp, 0, 0, NULL, NULL, OCI_DEFAULT);

	if(rc != 0)
	{
		SetError();
		_ociHandleFree(_stmtp_cursor, OCI_HTYPE_STMT);

		_stmtp_cursor = NULL;
		TRACE("OCI OpenCursor() Failed");
		return rc;
	}

	// Define the number of columns
	rc = _ociAttrGet(_stmtp_cursor, OCI_HTYPE_STMT, &_cursor_cols_count, 0, OCI_ATTR_PARAM_COUNT, _errhp); 

	if(_cursor_cols_count > 0)
		_cursor_cols = new SqlCol[_cursor_cols_count];

	size_t row_size = 0;

	_cursor_lob_exists = false;

	// Get column information
	for(int i = 0; i < _cursor_cols_count; i++)
	{
		OCIParam *colhp;
		ub4 size = 0;
		char *name = NULL;

		rc = _ociParamGet(_stmtp_cursor, OCI_HTYPE_STMT, _errhp, (void**)&colhp, (ub4)(i + 1));

		// Get the pointer to column name
		rc = _ociAttrGet(colhp, OCI_DTYPE_PARAM, (void*)&name, &size, OCI_ATTR_NAME, _errhp);

		// Copy the column name
		if(size > 0)
		{
			strncpy(_cursor_cols[i]._name, name, size);
			_cursor_cols[i]._name[size] = '\x0';
		}

		// Get column native data type
		rc = _ociAttrGet(colhp, OCI_DTYPE_PARAM, (void*)&_cursor_cols[i]._native_dt, 0, OCI_ATTR_DATA_TYPE, _errhp);

		
		// Get column length in bytes for character and binary strings
		rc = _ociAttrGet(colhp, OCI_DTYPE_PARAM, (void*)&_cursor_cols[i]._len, 0, OCI_ATTR_DATA_SIZE, _errhp);

		// Check if length ratio applied for VARCHAR2 and CHAR columns
		if((_cursor_cols[i]._native_dt == SQLT_CHR || _cursor_cols[i]._native_dt == SQLT_AFC) && _char_length_ratio != 0.0 && !catalog_query)
			_cursor_cols[i]._len = _cursor_cols[i]._len * _char_length_ratio;

		if(_cursor_cols[i]._native_dt == SQLT_BLOB || _cursor_cols[i]._native_dt == SQLT_CLOB)
		{
            if(_cursor_fetch_lob_as_varchar)
            {
                _bind_long_inplace = 32000;
                _cursor_cols[i]._len = _bind_long_inplace;
            }
            else
            {
			    _cursor_cols[i]._lob = true;
			    _cursor_lob_exists = true;
            }
		}
		else
		// For LONG data type, 0 size is returned
		if(_cursor_cols[i]._native_dt == SQLT_LNG)
        {
            if(_cursor_fetch_lob_as_varchar)
            {
                _bind_long_inplace = 32000;
                _cursor_cols[i]._len = _bind_long_inplace;
            }
            else
			    _cursor_cols[i]._len = 4000;
        }
		else
		// Object type including XMLTYPE
		if(_cursor_cols[i]._native_dt == SQLT_NTY)
        {
            if(_cursor_fetch_lob_as_varchar)
            {
                _bind_long_inplace = 32000;
                _cursor_cols[i]._len = _bind_long_inplace;
            }
            else
			    _cursor_cols[i]._len = 4000;
        }

		row_size += _cursor_cols[i]._len;

		// Get precision and scale for NUMBER(p,s), precision is 0 and scale is 129 for NUMBER
		rc = _ociAttrGet(colhp, OCI_DTYPE_PARAM, (void*)&_cursor_cols[i]._precision, 0, OCI_ATTR_PRECISION, _errhp);
		rc = _ociAttrGet(colhp, OCI_DTYPE_PARAM, (void*)&_cursor_cols[i]._scale, 0, OCI_ATTR_SCALE, _errhp);

		// Get NOT NULL attribute
		ub1 nullable = 1;
		rc = _ociAttrGet(colhp, OCI_DTYPE_PARAM, (void*)&nullable, 0, OCI_ATTR_IS_NULL, _errhp);

		_cursor_cols[i]._nullable = (nullable == 1) ? true : false;

		rc = _ociDescriptorFree(colhp, OCI_DTYPE_PARAM);
	}

	_cursor_allocated_rows = 1;

    if(row_size == 0)
        row_size = 10000;

	// Define how many rows fetch at once
	if(buffer_rows > 0)
		_cursor_allocated_rows = buffer_rows;
	else
	if(buffer_memory > 0)
	{
		size_t rows = buffer_memory/row_size;
		_cursor_allocated_rows = rows > 0 ? rows : 1;
	}		

	// Check if max batch size in rows is limited
	if (_batch_max_rows > 0 && _cursor_allocated_rows > _batch_max_rows)
		_cursor_allocated_rows = _batch_max_rows;

	OCIDefine *defnpp;

	// Allocate buffers to each column
	for(int i = 0; i < _cursor_cols_count; i++)
	{
		// if data type is VARCHAR2 (SQLT_CHR) or CHAR (SQLT_AFC), bind to NULL terminated string
		if(_cursor_cols[i]._native_dt == SQLT_CHR || _cursor_cols[i]._native_dt == SQLT_AFC)
		{
			_cursor_cols[i]._native_fetch_dt = SQLT_STR;
			_cursor_cols[i]._fetch_len = _cursor_cols[i]._len + 1;
			_cursor_cols[i]._data = new char[_cursor_cols[i]._fetch_len * _cursor_allocated_rows];
		}
		else
		// Bind NUMBER data type without precision and scale to string
		if(_cursor_cols[i]._native_dt == SQLT_NUM && 
			((_cursor_cols[i]._precision == 0 && _cursor_cols[i]._scale == 0) ||
				_cursor_cols[i]._scale != 0 || _cursor_cols[i]._precision >= 10))
		{
			_cursor_cols[i]._native_fetch_dt = SQLT_STR;
			_cursor_cols[i]._fetch_len = 42;	// less value returns truncation error on max number
			_cursor_cols[i]._data = new char[_cursor_cols[i]._fetch_len * _cursor_allocated_rows];		
		}
		else
		// If data type is NUMBER but precision is less than 10 and scale 0 then bind to INT
		if(_cursor_cols[i]._native_dt == SQLT_NUM && 
			_cursor_cols[i]._precision < 10 && _cursor_cols[i]._scale == 0)
		{
			// Check whether target supports bind to INT or string has to be used
			if(_target_api_provider != NULL && _target_api_provider->IsIntegerBulkBindSupported() == true)
			{
				_cursor_cols[i]._native_fetch_dt = SQLT_INT;
				_cursor_cols[i]._fetch_len = sizeof(int);
			}
			else
			{
				_cursor_cols[i]._native_fetch_dt = SQLT_STR;
				_cursor_cols[i]._fetch_len = 11;
			}

			_cursor_cols[i]._data = new char[_cursor_cols[i]._fetch_len * _cursor_allocated_rows];
		}
		else
		// Oracle DATE SQLT_DAT takes 7 bytes
		if(_cursor_cols[i]._native_dt == SQLT_DAT)
		{
			_cursor_cols[i]._native_fetch_dt = SQLT_DAT;
			_cursor_cols[i]._data = new char[_cursor_cols[i]._len * _cursor_allocated_rows];
			_cursor_cols[i]._fetch_len = _cursor_cols[i]._len;
		}
		else
		// Oracle TIMESTAMP, TIMESTAMP WITH TIME ZONE, INTERVAL DAY TO SECOND and YEAR TO MONTH data types 
		if(_cursor_cols[i]._native_dt == SQLT_TIMESTAMP || _cursor_cols[i]._native_dt == SQLT_TIMESTAMP_TZ ||
			_cursor_cols[i]._native_dt == SQLT_INTERVAL_DS || _cursor_cols[i]._native_dt == SQLT_INTERVAL_YM)
		{
			_cursor_cols[i]._native_fetch_dt = SQLT_STR;
			
			// Value is fetched as string with maximum 9 digit fraction, allocate buffer to hold the largest value
			_cursor_cols[i]._fetch_len = 29 + 1;
			
			// Time zone region can be specified as string (US/Pacific)
			if(_cursor_cols[i]._native_dt == SQLT_TIMESTAMP_TZ)			
				_cursor_cols[i]._fetch_len += 31;

			_cursor_cols[i]._data = new char[_cursor_cols[i]._fetch_len * _cursor_allocated_rows];
		}
		else
		// Oracle RAW data type
		if(_cursor_cols[i]._native_dt == SQLT_BIN)
		{
			_cursor_cols[i]._native_fetch_dt = SQLT_BIN;
			_cursor_cols[i]._fetch_len = _cursor_cols[i]._len;
			_cursor_cols[i]._data = new char[_cursor_cols[i]._fetch_len * _cursor_allocated_rows];
		}
		else
		// Oracle BLOB and CLOB using locators
		if(_cursor_cols[i]._native_dt == SQLT_BLOB || _cursor_cols[i]._native_dt == SQLT_CLOB)
		{
            // Use in place read if variable is set
			if(_bind_long_inplace > 0)
			{
                _cursor_cols[i]._native_fetch_dt = SQLT_STR;
				_cursor_cols[i]._fetch_len = (size_t)_bind_long_inplace;
				_cursor_cols[i]._data = new char[_bind_long_inplace * _cursor_allocated_rows];
			}
            else
            {
			    _cursor_cols[i]._native_fetch_dt = _cursor_cols[i]._native_dt;
			    _cursor_cols[i]._fetch_len = sizeof(OCILobLocator*);

			    _cursor_cols[i]._data = new char[sizeof(OCILobLocator*) * _cursor_allocated_rows];

			    // Allocate LOB locators (OCIArrayDescriptorAlloc function is not available in 10g Client)
			    if(_ociArrayDescriptorAlloc != NULL)
				    rc = _ociArrayDescriptorAlloc(_envhp, (void**)_cursor_cols[i]._data, OCI_DTYPE_LOB, (ub4)_cursor_allocated_rows, 0, NULL);
			    else 
			    // write loop for 10g client here
			    {  }
            }
		}
		else
		// Oracle LONG 
		if(_cursor_cols[i]._native_dt == SQLT_LNG)
		{
			_cursor_cols[i]._native_fetch_dt = _cursor_cols[i]._native_dt;
			
			// Use in place read if variable is set
			if(_bind_long_inplace > 0)
			{
				_cursor_cols[i]._fetch_len = (size_t)_bind_long_inplace;
				_cursor_cols[i]._data = new char[_bind_long_inplace * _cursor_allocated_rows];
			}
		}
		else
		// Object type
		if(_cursor_cols[i]._native_dt == SQLT_NTY)
		{
			_cursor_cols[i]._native_fetch_dt = SQLT_STR;
			_cursor_cols[i]._fetch_len = _cursor_cols[i]._len;
			_cursor_cols[i]._data = new char[_cursor_cols[i]._fetch_len * _cursor_allocated_rows];
		}
		else
		// ROWID data type 
		if(_cursor_cols[i]._native_dt == SQLT_RDD)
		{
			_cursor_cols[i]._native_fetch_dt = SQLT_STR;
			
			// 18 bytes in character representation
			_cursor_cols[i]._fetch_len = 18 + 1;
			
			_cursor_cols[i]._data = new char[_cursor_cols[i]._fetch_len * _cursor_allocated_rows];
		}

		// Bind the data to array
		if(_cursor_cols[i]._data != NULL)
		{
			// Allocate indicators
			_cursor_cols[i]._ind2 = new short[_cursor_allocated_rows];
			_cursor_cols[i]._len_ind2 = new short[_cursor_allocated_rows];

			size_t fetch_len = _cursor_cols[i]._fetch_len;

			// For LOBs set fetch length to 0 for OCIDefinePos
			if(_bind_long_inplace == 0 &&
                (_cursor_cols[i]._native_dt == SQLT_BLOB || _cursor_cols[i]._native_dt == SQLT_CLOB))
				fetch_len = 0;

			rc = _ociDefineByPos(_stmtp_cursor, &defnpp, _errhp, (ub4)(i + 1), _cursor_cols[i]._data, (sb4)fetch_len, 
								(ub2)_cursor_cols[i]._native_fetch_dt, (ub2*)_cursor_cols[i]._ind2, 
								(ub2*)_cursor_cols[i]._len_ind2, NULL, OCI_DEFAULT);

            if(rc == -1)
	        {
		        // Set error message, close cursor and remove buffers
		        SetError();
		        CloseCursor();
				TRACE("OCI OpenCursor() Failed");
		        return -1;
	        }
		}
	}

	// Fetch the initial set of data
	rc = _ociStmtFetch2(_stmtp_cursor, _errhp, (ub4)_cursor_allocated_rows, OCI_DEFAULT, 0, OCI_DEFAULT);

	if(rc == -1)
	{
		// Set error message, close cursor and remove buffers
		SetError();
		CloseCursor();
		TRACE("OCI OpenCursor() Failed");
		return -1;
	}

	int fetched = 0;

	// Get the number of fetched rows
	_ociAttrGet(_stmtp_cursor, OCI_HTYPE_STMT, (void*)&fetched, NULL, OCI_ATTR_ROWS_FETCHED, _errhp);

	if(col_count != NULL)
		*col_count = _cursor_cols_count; 

	if(allocated_array_rows != NULL)
		*allocated_array_rows = _cursor_allocated_rows;

	if(rows_fetched != NULL)
		*rows_fetched = fetched;
	
	if(cols != NULL)
		*cols = _cursor_cols;

	if(time_spent != NULL)
		*time_spent = GetTickCount() - start;

	TRACE("OCI OpenCursor() Left");
	return rc;
}

// Fetch next portion of data to allocate buffers
int SqlOciApi::Fetch(int *rows_fetched, size_t *time_spent) 
{
	if(_ociStmtFetch2 == NULL || _ociAttrGet == NULL || _cursor_allocated_rows <= 0)
		return -1;

	TRACE("OCI Fetch() Entered");
	size_t start = GetTickCount();

	// Fetch the data
	int rc = _ociStmtFetch2(_stmtp_cursor, _errhp, (ub4)_cursor_allocated_rows, OCI_DEFAULT, 0, OCI_DEFAULT);

	int fetched = 0;

	// Get the number of fetched rows
	_ociAttrGet(_stmtp_cursor, OCI_HTYPE_STMT, (void*)&fetched, NULL, OCI_ATTR_ROWS_FETCHED, _errhp);

	if(rows_fetched != NULL)
		*rows_fetched = fetched;

	if(time_spent != NULL)
		*time_spent = GetTickCount() - start;
	
	TRACE("OCI Fetch() Left");
	return rc;
}

// Close the cursor and deallocate buffers
int SqlOciApi::CloseCursor()
{
	if(_ociHandleFree == NULL)
		return -1;

	TRACE("OCI CloseCursor() Entered");

	// Close the statement handle
	int rc = _ociHandleFree(_stmtp_cursor, OCI_HTYPE_STMT);

	_stmtp_cursor = NULL;

	if(_cursor_cols == NULL)
		return 0;

	// Delete allocated buffers
	for(int i = 0; i < _cursor_cols_count; i++)
	{
		delete [] _cursor_cols[i]._data;
		delete [] _cursor_cols[i]._ind2;
		delete [] _cursor_cols[i]._len_ind2;
	}

	delete [] _cursor_cols;

	_cursor_cols = NULL;
	_cursor_cols_count = 0;
	_cursor_allocated_rows = 0;

	TRACE("OCI CloseCursor() Left");
	return rc;
}

// Specifies whether API allows to parallel reading from this API and write to another API
bool SqlOciApi::CanParallelReadWrite()
{
	// LOB allocators are used so buffers cannot be just copied
	if(_cursor_lob_exists == true)
		return false;

	return true;
}

// Get the length of LOB column in the open cursor 
int SqlOciApi::GetLobLength(size_t row, size_t column, size_t *length)
{
	if(_cursor_cols == NULL || row >= _cursor_allocated_rows || column >=_cursor_cols_count || _ociLobGetLength2 == NULL)
		return -1;

	OCILobLocator *loc = ((OCILobLocator**)_cursor_cols[column]._data)[row];
	oraub8 len = 0;

	// Size is in bytes for BLOB, in characters for CLOB
	int rc = _ociLobGetLength2(_svchp, _errhp, loc, &len);

	if(rc < 0)
		SetError();

	if(length != NULL)
		*length = (size_t)len;

	return rc;
}

// Allocate the buffer to read the LOB value depending on character lengths
char* SqlOciApi::GetLobBuffer(size_t row, size_t column, size_t length, size_t *alloc_len) 
{ 
	if(_cursor_cols == NULL || row >= _cursor_allocated_rows || column >=_cursor_cols_count || length <= 0)
		return NULL;

	size_t buf_len = length;

	// For CLOB input length is in bytes, define buffer size according the character set
	if(_cursor_cols[column]._native_dt == SQLT_CLOB)
		buf_len = length * GetCharMaxSizeInBytes();

	char *buf = new char[buf_len];
	
	if(alloc_len != NULL)
		*alloc_len = buf_len;

	return buf; 
}

void SqlOciApi::FreeLobBuffer(char *buf) 
{ 
	if(buf != NULL)
		delete buf; 
}

// Get LOB content
int SqlOciApi::GetLobContent(size_t row, size_t column, void *data, size_t length, int *len_ind)
{
	if(data == NULL || _cursor_cols == NULL || _cursor_cols_count <= column || _ociLobRead2 == NULL)
		return -1;

	OCILobLocator *loc = ((OCILobLocator**)_cursor_cols[column]._data)[row];

	oraub8 len = (oraub8)length;
	int rc = 0;
	ub1 csfrm = 0;

	// When reading CLOB containing one byte in UTF-8, and when byte_amtp is used, input len must be 4, 
	// otherwise OCI_NEED_DATA returned. But after calling OCILobRead2 len will contain 1 (number if bytes)

	// To read CLOB csfrm must be set to 0 or SQLCS_IMPLICIT in OCILobRead2 call; to read NCLOB csfrm must be set SQLCS_NCHAR
	// otherwise ORA-24806: LOB form mismatch returned

	// Get SQLCS_IMPLICIT for CLOB and SQLCS_NCHAR for NCLOB locator (SQLT_CLOB type is returned for both CLOB and NCLOB)
	if(_cursor_cols[column]._native_dt == SQLT_CLOB)
		rc = _ociLobCharSetForm(_envhp, _errhp, loc, &csfrm);

	// Read the LOB value
	rc = _ociLobRead2(_svchp, _errhp, loc, &len, NULL, 1, data, (oraub8)length, OCI_ONE_PIECE, NULL, NULL, 0, csfrm);

	if(rc < 0)
		SetError();

	// Bytes read for both CLOB and BLOB when byte_amtp is used
	if(len_ind != NULL)
		*len_ind = (int)len;

	return rc;
}

// Get the list of available tables
int SqlOciApi::GetAvailableTables(std::string &select, std::string &exclude, 
										std::list<std::string> &tables)
{
	std::string condition;

	// Get a condition to select objects from the catalog
	GetSelectionCriteria(select.c_str(), exclude.c_str(), "owner", "table_name", condition, _user.c_str(), true);

	// Build the query
	std::string query = "SELECT owner, table_name FROM all_tables WHERE";

	// Exclude overflow tables for index organized tables
	// In any case, user cannot access them directly, Oracle returns ORA-25191: cannot reference overflow table of an index-organized table
	// IOT_NAME column available even in Oracle 9i
	query += " iot_name IS NULL";
	
	// Add filter
	if(condition.empty() == false)
	{
		query += " AND ";
		query += condition;
	}

	// Exclude system and Oracle specific schemas if *.* condition is set
	if(strcmp(select.c_str(), "*.*") == 0)
	{
		query += " AND owner NOT IN ('APPQOSSYS','DBSNMP','CTXSYS','FLOWS_FILES','MDSYS','OUTLN','SYS',";
		query += " 'SYSTEM','XDB')";
		query += " AND owner NOT LIKE 'APEX%'";
	}

	OCIStmt *stmtp;
	OCIDefine *defnpp;

	// Fetched data
	char owner[100][31];
	char table_name[100][31];

	// Fetched data indicators, lengths and codes
	sb2 owner_ind[100], table_name_ind[100];
	ub2 owner_len[100], table_name_len[100];
	ub2 owner_code[100], table_name_code[100];

	int prefetch_rows = 100;
	int fetched;

	int rc = _ociHandleAlloc((dvoid*)_envhp, (dvoid**)&stmtp, OCI_HTYPE_STMT, 0, NULL);

	// Set prefetch count
	rc = _ociAttrSet(stmtp, OCI_HTYPE_STMT, (void*)&prefetch_rows, sizeof(int), OCI_ATTR_PREFETCH_ROWS, _errhp);

	rc = _ociStmtPrepare(stmtp, _errhp, (text*)query.c_str(), (ub4)query.length(), OCI_NTV_SYNTAX, OCI_DEFAULT);

	// Define the select list items 
	rc = _ociDefineByPos(stmtp, &defnpp, _errhp, 1, (void*)owner, 31, SQLT_STR, (void*)owner_ind,
						owner_len, owner_code, OCI_DEFAULT);
	rc = _ociDefineByPos(stmtp, &defnpp, _errhp, 2, (void*)table_name, 31, SQLT_STR, (void*)table_name_ind,
						table_name_len, table_name_code, OCI_DEFAULT);
	
	// Execute the statement and perform the initial fetch of 100 rows into the defined array
	rc = _ociStmtExecute(_svchp, stmtp, _errhp, 100, 0, NULL, NULL, OCI_DEFAULT);

	if(rc < 0)
		SetError();
	
	while(rc >= 0)
	{
		_ociAttrGet(stmtp, OCI_HTYPE_STMT, (void*)&fetched, NULL, OCI_ATTR_ROWS_FETCHED, _errhp);

		// OCI_NO_DATA is returned by OCIStmtExecute and OCIStmtFetch2 when the number of fetched rows 
		// is less than the number of rows allocated in the array
		if(fetched == 0)
			break;

		// Output fetched data 
		for(int i = 0; i < fetched; i++)
		{
			std::string tab = owner[i];
			tab += ".";
			tab += table_name[i];

			tables.push_back(tab);
		}

		if(rc == OCI_NO_DATA)
			break;

		// Fetch another set of rows
		rc = _ociStmtFetch2(stmtp, _errhp, 100, OCI_DEFAULT, 0, OCI_DEFAULT);
	}

	rc = _ociHandleFree(stmtp, OCI_HTYPE_STMT);
		
	return 0;
}

// Read schema information
int SqlOciApi::ReadSchema(const char *select, const char *exclude, bool read_cns, bool read_idx)
{
	std::string selection;
	std::string selection2;

	ClearSchema();

	// Build WHERE clause to select rows from catalog
	GetSelectionCriteria(select, exclude, "owner", "table_name", selection, _user.c_str(), true);
	GetSelectionCriteria(select, exclude, "table_owner", "table_name", selection2, _user.c_str(), true);

	// Exclude system and Oracle specific schemas if *.* condition is set
	if(select != NULL && strcmp(select, "*.*") == 0 && selection.empty())
	{
		selection += " owner NOT IN ('APPQOSSYS','DBSNMP','CTXSYS','FLOWS_FILES','MDSYS','OUTLN','SYS',";
		selection += " 'SYSTEM','XDB')";
		selection += " AND owner NOT LIKE 'APEX%'";

		selection2 += " table_owner NOT IN ('APPQOSSYS','DBSNMP','CTXSYS','FLOWS_FILES','MDSYS','OUTLN','SYS',";
		selection2 += " 'SYSTEM','XDB')";
		selection2 += " AND table_owner NOT LIKE 'APEX%'";
	}

	int rc = ReadTableColumns(selection);
	rc = ReadComments(selection);

	if(read_cns)
	{
		rc = ReadTableConstraints(selection, "'P', 'U', 'R', 'C'");
		rc = ReadConstraintColumns(selection);
	}

	if(read_idx)
	{
		rc = ReadIndexes(selection2);
		rc = ReadIndColumns(selection2);
		rc = ReadIndExpressions(selection2);
	}

	rc = ReadSequences(selection); 
	return rc;
}

// Read schema when Oracle is the target database for transfer
int SqlOciApi::ReadSchemaForTransferTo(const char * /*select*/, const char * /*exclude*/)
{
	// Leave the filter empty as it is unknown how source objects are mapped to target objects
	std::string selection;

	ClearSchema();

	// Read table constraints as foreign keys will be removed before data transfer
	// There are a lot of CHECK constraints for NOT NULL, do not read them
	int rc = ReadTableConstraints(selection, "'P', 'U', 'R'");	

	return rc;
}

// Read catalog information about the table columns
int SqlOciApi::ReadTableColumns(std::string &selection)
{
	std::string query = "SELECT owner, table_name, column_name, data_type, data_default";
	query += " FROM all_tab_columns ";

	if(selection.empty() == false)
	{
		query += " WHERE ";
		query += selection;
	}

	query += " ORDER BY owner, table_name, column_id";

	// Bind LONG data_default column in place
    size_t long_inplace_old = _bind_long_inplace;
	_bind_long_inplace = 8000;

	size_t col_count = 0;
	size_t allocated_rows = 0;
	int rows_fetched = 0; 
	size_t time_read = 0;
	
	SqlCol *cols = NULL;
	SqlColMeta col_meta;

	// Open cursor allocating 100 rows buffer
	int rc = OpenCursor(query.c_str(), 100, 0, &col_count, &allocated_rows, &rows_fetched, &cols, &time_read);

	while(rc >= 0)
	{
		// Copy column information
		for(int i = 0; i < rows_fetched; i++)
		{
			// Schema
			if(cols[0]._ind2[i] != -1)
			{
				col_meta.schema = new char[(size_t)cols[0]._len_ind2[i] + 1];

				strncpy(col_meta.schema, cols[0]._data + cols[0]._fetch_len * i, (size_t)cols[0]._len_ind2[i]);
				col_meta.schema[cols[0]._len_ind2[i]] = '\x0';
			}
			else
				col_meta.schema = NULL;

			// Table
			if(cols[1]._ind2[i] != -1)
			{
				col_meta.table = new char[(size_t)cols[1]._len_ind2[i] + 1];

				strncpy(col_meta.table, cols[1]._data + cols[1]._fetch_len * i, (size_t)cols[1]._len_ind2[i]);
				col_meta.table[cols[1]._len_ind2[i]] = '\x0';
			}
			else
				col_meta.table = NULL;

			// Column
			if(cols[2]._ind2[i] != -1)
			{
				col_meta.column = new char[(size_t)cols[2]._len_ind2[i] + 1];

				strncpy(col_meta.column, cols[2]._data + cols[2]._fetch_len * i, (size_t)cols[2]._len_ind2[i]);
				col_meta.column[cols[2]._len_ind2[i]] = '\x0';
			}
			else
				col_meta.column = NULL;

			// Data type
			if(cols[3]._ind2[i] != -1)
			{
				col_meta.data_type = new char[(size_t)cols[3]._len_ind2[i] + 1];

				strncpy(col_meta.data_type, cols[3]._data + cols[3]._fetch_len * i, (size_t)cols[3]._len_ind2[i]);
				col_meta.data_type[cols[3]._len_ind2[i]] = '\x0';
			}
			else
				col_meta.data_type = NULL;

			// Default value (String literal contains single quotes)
			if(cols[4]._ind2[i] != -1)
			{
				col_meta.default_value = new char[(size_t)cols[4]._len_ind2[i] + 1];

				strncpy(col_meta.default_value, cols[4]._data + cols[4]._fetch_len * i, (size_t)cols[4]._len_ind2[i]);
				col_meta.default_value[cols[4]._len_ind2[i]] = '\x0';

				col_meta.no_default = false;
			}
			else
			{
				col_meta.no_default = true;
				col_meta.default_value = NULL; 
			}

			_table_columns.push_back(col_meta);
		}

		// No more rows
		if(rc == 100)
			break;

		rc = Fetch(&rows_fetched, &time_read);
	}

	// Set pointer to NULL to avoid delete if destructor (values belong to list now)
	col_meta.schema = NULL; 
	col_meta.table = NULL; 
	col_meta.column = NULL; 
	col_meta.data_type = NULL; 
	col_meta.default_value = NULL; 

	CloseCursor();

	_bind_long_inplace = long_inplace_old;

	return 0;
}

// Read catalog information about the table constraints
int SqlOciApi::ReadTableConstraints(std::string &selection, const char *types)
{
	std::string query = "SELECT owner, table_name, constraint_name, constraint_type, search_condition, r_owner, r_constraint_name";
	query += " FROM all_constraints WHERE constraint_type in (";
	query += types;
	query += ") AND constraint_name NOT LIKE 'BIN$%'";

	if(selection.empty() == false)
	{
		query += " AND ";
		query += selection;
	}
	
	// Bind LONG data_default column in place
    size_t long_inplace_old = _bind_long_inplace;
	_bind_long_inplace = 8000;

	size_t col_count = 0;
	size_t allocated_rows = 0;
	int rows_fetched = 0; 
	size_t time_read = 0;
	
	SqlCol *cols = NULL;
	SqlConstraints col_cns;

	// Open cursor allocating 100 rows buffer
	int rc = OpenCursor(query.c_str(), 100, 0, &col_count, &allocated_rows, &rows_fetched, &cols, &time_read);

	while(rc >= 0)
	{
		// Copy column information
		for(int i = 0; i < rows_fetched; i++)
		{
			col_cns.schema = NULL; 
			col_cns.table = NULL; 
			col_cns.constraint = NULL; 
			col_cns.condition = NULL; 
			col_cns.r_schema = NULL; 
			col_cns.r_constraint = NULL;

			// Get constraint type
			col_cns.type = (cols[3]._ind2[i] != -1) ? *(cols[3]._data + cols[3]._fetch_len * i) : '\x0';

			// If it is a CHECK constraint, skip "COL_NAME" IS NOT NULL
			if(col_cns.type == 'C')
			{
				char *data = (cols[4]._ind2[i] != -1) ? (cols[4]._data + cols[4]._fetch_len * i) : NULL;
				int len = cols[4]._len_ind2[i];

				bool not_null = false;

				if(data != NULL)
				{
					int k = len;
					char *cur = data;

					if(*cur == '"' && k > 0)
					{
						cur++; k--;

						// Skip column name
						while(*cur != '"' && k > 0)
						{
							cur++; k--;
						}

						if(*cur == '"' && k > 0)
						{
							cur += 2; k -= 2;

							// cur must point to IS NOT NULL
							if(strncmp(cur, "IS NOT NULL", (size_t)k) == 0)
								not_null = true;
						}
					}

					if(not_null == true)
						continue;

					col_cns.condition = new char[(size_t)len + 1];

					strncpy(col_cns.condition, data, (size_t)len);
					col_cns.condition[len] = '\x0';
				}
			}

			// Schema
			if(cols[0]._ind2[i] != -1)
			{
				col_cns.schema = new char[(size_t)cols[0]._len_ind2[i] + 1];

				strncpy(col_cns.schema, cols[0]._data + cols[0]._fetch_len * i, (size_t)cols[0]._len_ind2[i]);
				col_cns.schema[cols[0]._len_ind2[i]] = '\x0';
			}

			// Table
			if(cols[1]._ind2[i] != -1)
			{
				col_cns.table = new char[(size_t)cols[1]._len_ind2[i] + 1];

				strncpy(col_cns.table, cols[1]._data + cols[1]._fetch_len * i, (size_t)cols[1]._len_ind2[i]);
				col_cns.table[cols[1]._len_ind2[i]] = '\x0';
			}

			// Constraint name
			if(cols[2]._ind2[i] != -1)
			{
				col_cns.constraint = new char[(size_t)cols[2]._len_ind2[i] + 1];

				strncpy(col_cns.constraint, cols[2]._data + cols[2]._fetch_len * i, (size_t)cols[2]._len_ind2[i]);
				col_cns.constraint[cols[2]._len_ind2[i]] = '\x0';
			}

			// Reference schema for foreing key constraints
			if(col_cns.type == 'R' && cols[5]._ind2[i] != -1)
			{
				col_cns.r_schema = new char[(size_t)cols[5]._len_ind2[i] + 1];

				strncpy(col_cns.r_schema, cols[5]._data + cols[5]._fetch_len * i, (size_t)cols[5]._len_ind2[i]);
				col_cns.r_schema[cols[5]._len_ind2[i]] = '\x0';
			}

			// Reference (primary key or unique) constraint name for foreign key constraints 
			if(col_cns.type == 'R' && cols[6]._ind2[i] != -1)
			{
				col_cns.r_constraint = new char[(size_t)cols[6]._len_ind2[i] + 1];

				strncpy(col_cns.r_constraint, cols[6]._data + cols[6]._fetch_len * i, (size_t)cols[6]._len_ind2[i]);
				col_cns.r_constraint[cols[6]._len_ind2[i]] = '\x0';
			}

			_table_constraints.push_back(col_cns);
		}

		// No more rows
		if(rc == 100)
			break;

		rc = Fetch(&rows_fetched, &time_read);
	}

	// Set pointer to NULL to avoid delete if destructor (values belong to list now)
	col_cns.schema = NULL; 
	col_cns.table = NULL; 
	col_cns.constraint = NULL; 
	col_cns.condition = NULL; 
	col_cns.r_schema = NULL; 
	col_cns.r_constraint = NULL;

	CloseCursor();

	_bind_long_inplace = long_inplace_old;

	return 0;
}

// Read catalog information about constraint columns
int SqlOciApi::ReadConstraintColumns(std::string &selection)
{
	// In Oracle, constraint name is unique within the schema (owner), no need to sort by table name
	std::string query = "SELECT owner, table_name, constraint_name, column_name";
	query += " FROM all_cons_columns ";

	if(selection.empty() == false)
	{
		query += " WHERE ";
		query += selection;
	}

	query += " ORDER BY owner, constraint_name, position";
	
	size_t col_count = 0;
	size_t allocated_rows = 0;
	int rows_fetched = 0; 
	size_t time_read = 0;
	
	SqlCol *cols = NULL;
	SqlConsColumns col_cns;

	// Open cursor allocating 100 rows buffer
	int rc = OpenCursor(query.c_str(), 100, 0, &col_count, &allocated_rows, &rows_fetched, &cols, &time_read);

	while(rc >= 0)
	{
		// Copy column information
		for(int i = 0; i < rows_fetched; i++)
		{
			// Schema
			if(cols[0]._ind2[i] != -1)
			{
				col_cns.schema = new char[(size_t)cols[0]._len_ind2[i] + 1];

				strncpy(col_cns.schema, cols[0]._data + cols[0]._fetch_len * i, (size_t)cols[0]._len_ind2[i]);
				col_cns.schema[cols[0]._len_ind2[i]] = '\x0';
			}
			else
				col_cns.schema = NULL;

			// Table
			if(cols[1]._ind2[i] != -1)
			{
				col_cns.table = new char[(size_t)cols[1]._len_ind2[i] + 1];

				strncpy(col_cns.table, cols[1]._data + cols[1]._fetch_len * i, (size_t)cols[1]._len_ind2[i]);
				col_cns.table[cols[1]._len_ind2[i]] = '\x0';
			}
			else
				col_cns.table = NULL;

			// Constraint name
			if(cols[2]._ind2[i] != -1)
			{
				col_cns.constraint = new char[(size_t)cols[2]._len_ind2[i] + 1];

				strncpy(col_cns.constraint, cols[2]._data + cols[2]._fetch_len * i, (size_t)cols[2]._len_ind2[i]);
				col_cns.constraint[cols[2]._len_ind2[i]] = '\x0';
			}
			else
				col_cns.constraint = NULL;

			// Column name
			if(cols[3]._ind2[i] != -1)
			{
				col_cns.column = new char[(size_t)cols[3]._len_ind2[i] + 1];

				strncpy(col_cns.column, cols[3]._data + cols[3]._fetch_len * i, (size_t)cols[3]._len_ind2[i]);
				col_cns.column[cols[3]._len_ind2[i]] = '\x0';
			}
			else
				col_cns.column = NULL;

			_table_cons_columns.push_back(col_cns);
		}

		// No more rows
		if(rc == 100)
			break;

		rc = Fetch(&rows_fetched, &time_read);
	}

	// Set pointer to NULL to avoid delete if destructor (values belong to list now)
	col_cns.schema = NULL; 
	col_cns.table = NULL; 
	col_cns.constraint = NULL; 
	col_cns.column = NULL; 

	CloseCursor();

	return 0;
}

// Get table name by constraint name
int SqlOciApi::ReadConstraintTable(const char *schema, const char *constraint, std::string &table)
{
	// In Oracle, constraint name is unique within the schema (owner)
	std::string query = "SELECT table_name";
	query += " FROM all_constraints WHERE ";
	
	if(schema != NULL)
	{
		query += "owner = '";
		query += schema;
		query += "' AND ";
	}

	if(constraint != NULL)
	{
		query += "constraint_name = '";
		query += constraint;
		query += "'";
	}
	
	size_t col_count = 0;
	size_t allocated_rows = 0;
	int rows_fetched = 0; 
	size_t time_read = 0;
	
	SqlCol *cols = NULL;
	char name[31];

	// Open cursor allocating 1 row buffer
	int rc = OpenCursor(query.c_str(), 1, 0, &col_count, &allocated_rows, &rows_fetched, &cols, &time_read);

	if(rc >= 0 && rows_fetched > 0)
	{
		// Table name
		if(cols[0]._ind2[0] != -1)
		{
			int len = (cols[0]._len_ind2[0] > 30) ? 30 : cols[0]._len_ind2[0];

			strncpy(name, cols[0]._data, (size_t)len);
			name[len] = '\x0';


			table = name;
		}
	}

	CloseCursor();

	return 0;
}

// Read constraint columns
int SqlOciApi::ReadConstraintColumns(const char *schema, const char *table, const char *constraint, std::string &columns)
{
	// In Oracle, constraint name is unique within the schema (owner), no need to find by table name
	std::string query = "SELECT column_name";
	query += " FROM all_cons_columns WHERE ";
	
	if(schema != NULL)
	{
		query += "owner = '";
		query += schema;
		query += "' AND ";
	}

	if(table != NULL)
	{
		query += "table_name = '";
		query += table;
		query += "' AND ";
	}

	if(constraint != NULL)
	{
		query += "constraint_name = '";
		query += constraint;
		query += "'";
	}

	query += " ORDER BY position";
	
	size_t col_count = 0;
	size_t allocated_rows = 0;
	int rows_fetched = 0; 
	size_t time_read = 0;
	
	SqlCol *cols = NULL;
	char col[31];

	// Open cursor allocating 100 rows buffer
	int rc = OpenCursor(query.c_str(), 100, 0, &col_count, &allocated_rows, &rows_fetched, &cols, &time_read);

	while(rc >= 0)
	{
		for(int i = 0; i < rows_fetched; i++)
		{
			// Column name
			if(cols[0]._ind2[i] != -1)
			{
				int len = (cols[0]._len_ind2[i] > 30) ? 30 : cols[0]._len_ind2[i];

				strncpy(col, cols[0]._data + cols[0]._fetch_len * i, (size_t)len);
				col[len] = '\x0';

				if(i != 0)
					columns += ", ";

				columns += col;
			}
		}

		// No more rows
		if(rc == 100)
			break;

		rc = Fetch(&rows_fetched, &time_read);
	}

	CloseCursor();

	return 0;
}

// Read catalog information about comments on tables, views and columns
int SqlOciApi::ReadComments(std::string &selection)
{
	std::string query = "SELECT owner, table_name, NULL, table_type, comments";
	query += " FROM all_tab_comments WHERE comments IS NOT NULL ";

	if(selection.empty() == false)
	{
		query += " AND ";
		query += selection;
	}

	query += " UNION ALL SELECT owner, table_name, column_name, NULL, comments"; 
	query += " FROM all_col_comments WHERE comments IS NOT NULL ";
	
	if(selection.empty() == false)
	{
		query += " AND ";
		query += selection;
	}

	size_t col_count = 0;
	size_t allocated_rows = 0;
	int rows_fetched = 0; 
	size_t time_read = 0;
	
	SqlCol *cols = NULL;
	SqlComments comment;

	// Open cursor allocating 100 rows buffer
	int rc = OpenCursor(query.c_str(), 100, 0, &col_count, &allocated_rows, &rows_fetched, &cols, &time_read);

	while(rc >= 0)
	{
		// Copy column information
		for(int i = 0; i < rows_fetched; i++)
		{
			// Schema
			if(cols[0]._ind2[i] != -1)
			{
				comment.schema = new char[(size_t)cols[0]._len_ind2[i] + 1];

				strncpy(comment.schema, cols[0]._data + cols[0]._fetch_len * i, (size_t)cols[0]._len_ind2[i]);
				comment.schema[cols[0]._len_ind2[i]] = '\x0';
			}
			else
				comment.schema = NULL;

			// Table
			if(cols[1]._ind2[i] != -1)
			{
				comment.table = new char[(size_t)cols[1]._len_ind2[i] + 1];

				strncpy(comment.table, cols[1]._data + cols[1]._fetch_len * i, (size_t)cols[1]._len_ind2[i]);
				comment.table[cols[1]._len_ind2[i]] = '\x0';
			}
			else
				comment.table = NULL;

			// Column
			if(cols[2]._ind2[i] != -1)
			{
				comment.column = new char[(size_t)cols[2]._len_ind2[i] + 1];

				strncpy(comment.column, cols[2]._data + cols[2]._fetch_len * i, (size_t)cols[2]._len_ind2[i]);
				comment.column[cols[2]._len_ind2[i]] = '\x0';
			}
			else
				comment.column = NULL;

			// Column type
			if(cols[3]._ind2[i] != -1)
			{
				if(strncmp(cols[3]._data + cols[3]._fetch_len * i, "TABLE", (size_t)cols[3]._len_ind2[i]) == 0)
					comment.type = 'T';
			}
			else
				comment.type = 'C';

			// Comment
			if(cols[4]._ind2[i] != -1)
			{
				comment.comment = new char[(size_t)cols[4]._len_ind2[i] + 1];

				strncpy(comment.comment, cols[4]._data + cols[4]._fetch_len * i, (size_t)cols[4]._len_ind2[i]);
				comment.comment[cols[4]._len_ind2[i]] = '\x0';
			}
			else
				comment.comment = NULL; 

			_table_comments.push_back(comment);
		}

		// No more rows
		if(rc == 100)
			break;

		rc = Fetch(&rows_fetched, &time_read);
	}

	// Set pointer to NULL to avoid delete if destructor (values belong to list now)
	comment.schema = NULL; 
	comment.table = NULL; 
	comment.column = NULL; 
	comment.comment = NULL; 

	CloseCursor();

	return 0;
}

// Read catalog information about indexes
int SqlOciApi::ReadIndexes(std::string &selection)
{
	// Generated='Y' means that name is generated, not index itself, so if CONSTRAINT name is given 
	// for PRIMARY KEY and UNIQUE generated contains 'N' and additional check required
	std::string query = "SELECT owner, index_name, table_owner, table_name, uniqueness";
	query += " FROM all_indexes WHERE generated = 'N' ";
	
	if(selection.empty() == false)
	{
		query += " AND ";
		query += selection;
	}
	
	size_t col_count = 0;
	size_t allocated_rows = 0;
	int rows_fetched = 0; 
	size_t time_read = 0;
	
	SqlCol *cols = NULL;
	SqlIndexes idx;

	// Open cursor allocating 100 rows buffer
	int rc = OpenCursor(query.c_str(), 100, 0, &col_count, &allocated_rows, &rows_fetched, &cols, &time_read);

	while(rc >= 0)
	{
		// Copy indexes information
		for(int i = 0; i < rows_fetched; i++)
		{
			idx.schema = NULL; 
			idx.index = NULL; 
			idx.t_schema = NULL; 
			idx.t_name = NULL; 	

			// Schema
			if(cols[0]._ind2[i] != -1)
			{
				idx.schema = new char[(size_t)cols[0]._len_ind2[i] + 1];

				strncpy(idx.schema, cols[0]._data + cols[0]._fetch_len * i, (size_t)cols[0]._len_ind2[i]);
				idx.schema[cols[0]._len_ind2[i]] = '\x0';
			}

			// Index name
			if(cols[1]._ind2[i] != -1)
			{
				idx.index = new char[(size_t)cols[1]._len_ind2[i] + 1];

				strncpy(idx.index, cols[1]._data + cols[1]._fetch_len * i, (size_t)cols[1]._len_ind2[i]);
				idx.index[cols[1]._len_ind2[i]] = '\x0';
			}

			// Table schema
			if(cols[2]._ind2[i] != -1)
			{
				idx.t_schema = new char[(size_t)cols[2]._len_ind2[i] + 1];

				strncpy(idx.t_schema, cols[2]._data + cols[2]._fetch_len * i, (size_t)cols[2]._len_ind2[i]);
				idx.t_schema[cols[2]._len_ind2[i]] = '\x0';
			}

			// Table name
			if(cols[3]._ind2[i] != -1)
			{
				idx.t_name = new char[(size_t)cols[3]._len_ind2[i] + 1];

				strncpy(idx.t_name, cols[3]._data + cols[3]._fetch_len * i, (size_t)cols[3]._len_ind2[i]);
				idx.t_name[cols[3]._len_ind2[i]] = '\x0';
			}

			// Uniqueness
			if(cols[4]._ind2[i] != -1 && strncmp(cols[4]._data + cols[4]._fetch_len * i, "UNIQUE", 6) == 0)
				idx.unique = true;
			else
				idx.unique = false;

			bool unique_generated_index = false;

			// For unique indexes make sure there is no unique constraint with the same name
			if(idx.unique)
			{
				SqlConstraints *cns = GetConstraint(idx.t_schema, idx.t_name, idx.index);

				if(cns != NULL && (cns->type == 'P' || cns->type == 'U'))
					unique_generated_index = true;
			}

			if(unique_generated_index == false)
				_table_indexes.push_back(idx);
			else
			{
				delete idx.schema; 
				delete idx.index; 
				delete idx.t_schema;
				delete idx.t_name;
			}
		}

		// No more rows
		if(rc == 100)
			break;

		rc = Fetch(&rows_fetched, &time_read);
	}

	// Set pointer to NULL to avoid delete if destructor (values belong to list now)
	idx.schema = NULL; 
	idx.index = NULL; 
	idx.t_schema = NULL; 
	idx.t_name = NULL; 	

	CloseCursor();

	return 0;
}

// Read catalog information about index columns
int SqlOciApi::ReadIndColumns(std::string &selection)
{
	// Skip system-generated indexes for primary and unqiue keys
	std::string query = "SELECT index_owner, index_name, column_name, descend";
	query += " FROM all_ind_columns WHERE index_name NOT LIKE 'SYS_C0%'";

	if(selection.empty() == false)
	{
		query += " AND ";
		query += selection;
	}

	query += " ORDER BY index_owner, index_name, column_position";
	
	size_t col_count = 0;
	size_t allocated_rows = 0;
	int rows_fetched = 0; 
	size_t time_read = 0;
	
	SqlCol *cols = NULL;
	SqlIndColumns idx;

	// Open cursor allocating 100 rows buffer
	int rc = OpenCursor(query.c_str(), 100, 0, &col_count, &allocated_rows, &rows_fetched, &cols, &time_read);

	while(rc >= 0)
	{
		// Copy column information
		for(int i = 0; i < rows_fetched; i++)
		{
			// Index schema
			if(cols[0]._ind2[i] != -1)
			{
				idx.schema = new char[(size_t)cols[0]._len_ind2[i] + 1];

				strncpy(idx.schema, cols[0]._data + cols[0]._fetch_len * i, (size_t)cols[0]._len_ind2[i]);
				idx.schema[cols[0]._len_ind2[i]] = '\x0';
			}
			else
				idx.schema = NULL;

			// Index name
			if(cols[1]._ind2[i] != -1)
			{
				idx.index = new char[(size_t)cols[1]._len_ind2[i] + 1];

				strncpy(idx.index, cols[1]._data + cols[1]._fetch_len * i, (size_t)cols[1]._len_ind2[i]);
				idx.index[cols[1]._len_ind2[i]] = '\x0';
			}
			else
				idx.index = NULL;

			// Index column
			if(cols[2]._ind2[i] != -1)
			{
				idx.column = new char[(size_t)cols[2]._len_ind2[i] + 1];

				strncpy(idx.column, cols[2]._data + cols[2]._fetch_len * i, (size_t)cols[2]._len_ind2[i]);
				idx.column[cols[2]._len_ind2[i]] = '\x0';
			}
			else
				idx.column = NULL;

			// Ascending/descending order
			if(cols[3]._ind2[i] != -1 && strncmp(cols[3]._data + cols[3]._fetch_len * i, "DESC", 4) == 0)
				idx.asc = false;
			else
				idx.asc = true;

			_table_ind_columns.push_back(idx);
		}

		// No more rows
		if(rc == 100)
			break;

		rc = Fetch(&rows_fetched, &time_read);
	}

	// Set pointer to NULL to avoid delete if destructor (values belong to list now)
	idx.schema = NULL; 
	idx.index = NULL; 
	idx.column = NULL;

	CloseCursor();

	return 0;
}

// Read catalog information about index expressions (for function-based indexes)
int SqlOciApi::ReadIndExpressions(std::string &selection)
{
	// Skip system-generated indexes for primary and unqiue keys
	std::string query = "SELECT index_owner, index_name, column_expression";
	query += " FROM all_ind_expressions ";
	
	if(selection.empty() == false)
	{
		query += " WHERE ";
		query += selection;
	}

	query += " AND index_name NOT LIKE 'SYS_C0%'";
	query += " ORDER BY index_owner, index_name, column_position";

	// Bind LONG column_expression column in place
    size_t long_inplace_old = _bind_long_inplace;
	_bind_long_inplace = 8000;
	
	size_t col_count = 0;
	size_t allocated_rows = 0;
	int rows_fetched = 0; 
	size_t time_read = 0;
	
	SqlCol *cols = NULL;
	SqlIndExp idx;

	// Open cursor allocating 100 rows buffer
	int rc = OpenCursor(query.c_str(), 100, 0, &col_count, &allocated_rows, &rows_fetched, &cols, &time_read);

	while(rc >= 0)
	{
		// Copy column information
		for(int i = 0; i < rows_fetched; i++)
		{
			// Index schema
			if(cols[0]._ind2[i] != -1)
			{
				idx.schema = new char[(size_t)cols[0]._len_ind2[i] + 1];

				strncpy(idx.schema, cols[0]._data + cols[0]._fetch_len * i, (size_t)cols[0]._len_ind2[i]);
				idx.schema[cols[0]._len_ind2[i]] = '\x0';
			}
			else
				idx.schema = NULL;

			// Index name
			if(cols[1]._ind2[i] != -1)
			{
				idx.index = new char[(size_t)cols[1]._len_ind2[i] + 1];

				strncpy(idx.index, cols[1]._data + cols[1]._fetch_len * i, (size_t)cols[1]._len_ind2[i]);
				idx.index[cols[1]._len_ind2[i]] = '\x0';
			}
			else
				idx.index = NULL;

			// Column expression
			if(cols[2]._ind2[i] != -1)
			{
				idx.expression = new char[(size_t)cols[2]._len_ind2[i] + 1];

				strncpy(idx.expression, cols[2]._data + cols[2]._fetch_len * i, (size_t)cols[2]._len_ind2[i]);
				idx.expression[cols[2]._len_ind2[i]] = '\x0';
			}
			else
				idx.expression = NULL;

			_table_ind_expressions.push_back(idx);
		}

		// No more rows
		if(rc == 100)
			break;

		rc = Fetch(&rows_fetched, &time_read);
	}

	// Set pointer to NULL to avoid delete if destructor (values belong to list now)
	idx.schema = NULL; 
	idx.index = NULL; 
	idx.expression = NULL;

	CloseCursor();

	_bind_long_inplace = long_inplace_old;

	return 0;
}

// Get a list of columns for specified primary or unique key
int SqlOciApi::GetKeyConstraintColumns(SqlConstraints &cns, std::list<std::string> &output, std::list<std::string> *types)
{
	bool found = false;

	if(cns.schema == NULL || cns.table == NULL || cns.constraint == NULL)
		return -1;

	for(std::list<SqlConsColumns>::iterator i = _table_cons_columns.begin(); i != _table_cons_columns.end(); i++)
	{
		char *s = (*i).schema;
		char *t = (*i).table;
		char *c = (*i).constraint;
		char *col = (*i).column;

		if(s == NULL || t == NULL || c == NULL || col == NULL)
			continue;

		// Compare schema, table and constraint names (columns are already ordered)
		if(strcmp(cns.constraint, c) == 0 && strcmp(cns.table, t) == 0 && strcmp(cns.schema, s) == 0)
		{
			output.push_back(col);
			found = true;

			// Find data type
			if(types != NULL)
			{
				for(std::list<SqlColMeta>::iterator m = _table_columns.begin(); m != _table_columns.end(); m++)
				{
					char *cs = (*m).schema;
					char *ct = (*m).table;
					char *ccol = (*m).column;
					char *dt = (*m).data_type;

					if(cs == NULL || ct == NULL || ccol == NULL || dt == NULL)
						break;

					if(strcmp(cs, s) == 0 && strcmp(ct, t) == 0 && strcmp(ccol, col) == 0)
					{
						if(!_stricmp(dt, "CHAR") || !_stricmp(dt, "VARCHAR2") || !_stricmp(dt, "VARCHAR"))
							types->push_back("String");
						else
							types->push_back(dt);
						
						break;
					}
				}
			}
		}
		else
		if(found)
			break;
	}

	return 0;
}

// Get a list of columns for specified index
int SqlOciApi::GetIndexColumns(SqlIndexes &idx, std::list<std::string> &idx_cols, std::list<std::string> &idx_sorts)
{
	int c = 0;

	// Find index columns
	for(std::list<SqlIndColumns>::iterator i = _table_ind_columns.begin(); i != _table_ind_columns.end(); i++)
	{
		// Compare schema and index names (columns are already ordered)
		if((*i).schema != NULL && strcmp((*i).schema, idx.schema) == 0 &&
			(*i).index != NULL && strcmp((*i).index, idx.index) == 0)
		{
			std::string column;
			std::string sort;

			// For DESC columns Oracle uses an expression, find the real column name
			if((*i).asc == false && strncmp((*i).column, "SYS_NC0", 7) == 0)
			{
				int p = 0;
				
				// Find the expression with the same position
				for(std::list<SqlIndExp>::iterator k = _table_ind_expressions.begin(); k != _table_ind_expressions.end(); k++)
				{
					if((*k).schema != NULL && strcmp((*k).schema, idx.schema) == 0 &&
						(*k).index != NULL && strcmp((*k).index, idx.index) == 0)
					{
						if(p != c)
						{
							p++; 
							continue;
						}

						char *cur = (*k).expression;

						// Column name is enclosed with quotes, remove them
						if(cur && *cur == '"')
						{
							cur++;
							while(*cur != '"') { column += *cur; cur++; }
						}

						break;
					}
				}
			}
			else
				column = (*i).column;

			if((*i).asc == false)
				sort = "DESC";

			idx_cols.push_back(column);
			idx_sorts.push_back(sort);
			c++;
		}
		else
		// Index columns ended
		if(c > 0)
			break;
	}

	return 0;
}

// Get column default by table and column name
int SqlOciApi::GetColumnDefault(const char *table, const char *column, std::string & /*col_default*/)
{
	if(table == NULL || column == NULL)
		return -1;

	std::string schema;
	std::string object;

	SplitQualifiedName(table, schema, object);

	// Find table columns (already ordered by column number)
	/*for(std::list<SqlColMeta>::iterator i = _table_columns.begin(); i != _table_columns.end(); i++)
	{
		if((*i).no_default)
			continue;

		char *s = (*i).schema;
		char *t = (*i).table;
		char *c = (*i).column;
		char *cm = (*i).comment;
		
		// Table found
		if(s != NULL && t != NULL && strcmp(s, schema.c_str()) == 0 && strcmp(t, object.c_str()) == 0)
		{
			// Column found
			if(column != NULL && c != NULL && cm != NULL && strcmp(c, column) == 0)
			{
				comment = cm;
				break;
			}
			else
			// Table comment
			if(column == NULL && c == NULL && cm != NULL && (*i).type == 'T')
			{
				comment = cm;
				break;
			}
		}
	}*/

	return 0;
}

// Get table or column comment by table and column name
int SqlOciApi::GetComment(const char *table, const char *column, std::string &comment) 
{
	// Column can be NULL if table comment is required
	if(table == NULL)
		return -1;

	std::string schema;
	std::string object;

	SplitQualifiedName(table, schema, object);

	// Find table columns (already ordered by column number)
	for(std::list<SqlComments>::iterator i = _table_comments.begin(); i != _table_comments.end(); i++)
	{
		char *s = (*i).schema;
		char *t = (*i).table;
		char *c = (*i).column;
		char *cm = (*i).comment;
		
		// Table found
		if(s != NULL && t != NULL && strcmp(s, schema.c_str()) == 0 && strcmp(t, object.c_str()) == 0)
		{
			// Column found
			if(column != NULL && c != NULL && cm != NULL && strcmp(c, column) == 0)
			{
				comment = cm;
				break;
			}
			else
			// Table comment
			if(column == NULL && c == NULL && cm != NULL && (*i).type == 'T')
			{
				comment = cm;
				break;
			}
		}
	}

	return 0;
}

// Read catalog information about sequences
int SqlOciApi::ReadSequences(std::string &selection)
{
	std::string seq_selection = selection;
	Str::ReplaceFirst(seq_selection, "owner", "sequence_owner"); 
	Str::ReplaceFirst(seq_selection, "AND table_name", "AND sequence_name"); 

	std::string query = "SELECT sequence_owner, sequence_name, min_value, max_value, increment_by, last_number, cache_size, "\
						"cycle_flag "\
						"FROM all_sequences ";
	
	if(seq_selection.empty() == false)
	{
		query += " WHERE ";
		query += seq_selection;
	}

	size_t col_count = 0;
	size_t allocated_rows = 0;
	int rows_fetched = 0; 
	size_t time_read = 0;
	
	SqlCol *cols = NULL;
	SqlSequences seq;

	// Open cursor allocating 100 rows buffer
	int rc = OpenCursor(query.c_str(), 100, 0, &col_count, &allocated_rows, &rows_fetched, &cols, &time_read);

	while(rc >= 0)
	{
		// Copy column information
		for(int i = 0; i < rows_fetched; i++)
		{
			// Schema
			if(cols[0]._ind2[i] != -1)
			{
				seq.schema = new char[(size_t)cols[0]._len_ind2[i] + 1];

				strncpy(seq.schema, cols[0]._data + cols[0]._fetch_len * i, (size_t)cols[0]._len_ind2[i]);
				seq.schema[cols[0]._len_ind2[i]] = '\x0';
			}
			else
				seq.schema = NULL;

			// Sequence name
			if(cols[1]._ind2[i] != -1)
			{
				seq.name = new char[(size_t)cols[1]._len_ind2[i] + 1];

				strncpy(seq.name, cols[1]._data + cols[1]._fetch_len * i, (size_t)cols[1]._len_ind2[i]);
				seq.name[cols[1]._len_ind2[i]] = '\x0';
			}
			else
				seq.name = NULL;

			// Min value
			if(cols[2]._ind2[i] != -1)
			{
				seq.min_str = new char[(size_t)cols[2]._len_ind2[i] + 1];

				strncpy(seq.min_str, cols[2]._data + cols[2]._fetch_len * i, (size_t)cols[2]._len_ind2[i]);
				seq.min_str[cols[2]._len_ind2[i]] = '\x0';
			}
			else
				seq.min_str = NULL;

			// Max value
			if(cols[3]._ind2[i] != -1)
			{
				seq.max_str = new char[(size_t)cols[3]._len_ind2[i] + 1];

				strncpy(seq.max_str, cols[3]._data + cols[3]._fetch_len * i, (size_t)cols[3]._len_ind2[i]);
				seq.max_str[cols[3]._len_ind2[i]] = '\x0';

				// Check for no max value
				if(strcmp(seq.max_str, "999999999999999999999999999") == 0)
					seq.no_max = true;
				else
					seq.no_max = false;
			}
			else
				seq.max_str = NULL;

			// Inc value
			if(cols[4]._ind2[i] != -1)
			{
				seq.inc_str = new char[(size_t)cols[4]._len_ind2[i] + 1];

				strncpy(seq.inc_str, cols[4]._data + cols[4]._fetch_len * i, (size_t)cols[4]._len_ind2[i]);
				seq.inc_str[cols[4]._len_ind2[i]] = '\x0';
			}
			else
				seq.inc_str = NULL;

			// Current value
			if(cols[5]._ind2[i] != -1)
			{
				seq.cur_str = new char[(size_t)cols[5]._len_ind2[i] + 1];

				strncpy(seq.cur_str, cols[5]._data + cols[5]._fetch_len * i, (size_t)cols[5]._len_ind2[i]);
				seq.cur_str[cols[5]._len_ind2[i]] = '\x0';
			}
			else
				seq.cur_str = NULL;

			// Cache size
			if(cols[6]._ind2[i] != -1)
			{
				seq.cache_size_str = new char[(size_t)cols[6]._len_ind2[i] + 1];

				strncpy(seq.cache_size_str, cols[6]._data + cols[6]._fetch_len * i, (size_t)cols[6]._len_ind2[i]);
				seq.cache_size_str[cols[6]._len_ind2[i]] = '\x0';
			}
			else
				seq.cache_size_str = NULL;

			// Cycle
			if(cols[7]._ind2[i] != -1)
			{
				char *data = cols[7]._data + cols[7]._fetch_len * i;

				if(*data == 'Y')
					seq.cycle = true;
				else
					seq.cycle = false;
			}
			else
				seq.cycle = false;

			_sequences.push_back(seq);
		}

		// No more rows
		if(rc == 100)
			break;

		rc = Fetch(&rows_fetched, &time_read);
	}

	// Set pointer to NULL to avoid delete if destructor (values belong to list now)
	seq.schema = NULL; 
	seq.name = NULL; 
	seq.min_str = NULL; 
	seq.max_str = NULL; 
	seq.inc_str = NULL; 
	seq.cur_str = NULL; 
	seq.cache_size_str = NULL;

	CloseCursor();

	return 0;
}

// Fill reserved words
int SqlOciApi::ReadReservedWords()
{
	// Reserved words not allowed in identifiers
	_reserved_words_ddl.push_back("ACCESS");                                                                          
	_reserved_words_ddl.push_back("ADD");                                                                             
	_reserved_words_ddl.push_back("ALL");                                                                             
	_reserved_words_ddl.push_back("ALTER");                                                                           
	_reserved_words_ddl.push_back("AND");                                                                             
	_reserved_words_ddl.push_back("ANY");                                                                             
	_reserved_words_ddl.push_back("AS");                                                                              
	_reserved_words_ddl.push_back("ASC");                                                                             
	_reserved_words_ddl.push_back("AUDIT");                                                                           
	_reserved_words_ddl.push_back("BETWEEN");                                                                         
	_reserved_words_ddl.push_back("BY");                                                                              
	_reserved_words_ddl.push_back("CHAR");                                                                            
	_reserved_words_ddl.push_back("CHECK");                                                                           
	_reserved_words_ddl.push_back("CLUSTER");                                                                         
	_reserved_words_ddl.push_back("COLUMN");                                                                          
	_reserved_words_ddl.push_back("COMMENT");                                                                         
	_reserved_words_ddl.push_back("COMPRESS");                                                                        
	_reserved_words_ddl.push_back("CONNECT");                                                                         
	_reserved_words_ddl.push_back("CREATE");                                                                          
	_reserved_words_ddl.push_back("CURRENT");                                                                         
	_reserved_words_ddl.push_back("DATE");                                                                            
	_reserved_words_ddl.push_back("DECIMAL");                                                                         
	_reserved_words_ddl.push_back("DEFAULT");                                                                         
	_reserved_words_ddl.push_back("DELETE");                                                                          
	_reserved_words_ddl.push_back("DESC");                                                                            
	_reserved_words_ddl.push_back("DISTINCT");                                                                        
	_reserved_words_ddl.push_back("DROP");                                                                            
	_reserved_words_ddl.push_back("ELSE");                                                                            
	_reserved_words_ddl.push_back("EXCLUSIVE");                                                                       
	_reserved_words_ddl.push_back("EXISTS");                                                                          
	_reserved_words_ddl.push_back("FILE");                                                                            
	_reserved_words_ddl.push_back("FLOAT");                                                                           
	_reserved_words_ddl.push_back("FOR");                                                                             
	_reserved_words_ddl.push_back("FROM");                                                                            
	_reserved_words_ddl.push_back("GRANT");                                                                           
	_reserved_words_ddl.push_back("GROUP");                                                                           
	_reserved_words_ddl.push_back("HAVING");                                                                          
	_reserved_words_ddl.push_back("IDENTIFIED");                                                                      
	_reserved_words_ddl.push_back("IMMEDIATE");                                                                       
	_reserved_words_ddl.push_back("IN");                                                                              
	_reserved_words_ddl.push_back("INCREMENT");                                                                       
	_reserved_words_ddl.push_back("INDEX");                                                                           
	_reserved_words_ddl.push_back("INITIAL");                                                                         
	_reserved_words_ddl.push_back("INSERT");                                                                          
	_reserved_words_ddl.push_back("INTEGER");                                                                         
	_reserved_words_ddl.push_back("INTERSECT");                                                                       
	_reserved_words_ddl.push_back("INTO");                                                                            
	_reserved_words_ddl.push_back("IS");                                                                              
	_reserved_words_ddl.push_back("LEVEL");                                                                           
	_reserved_words_ddl.push_back("LIKE");                                                                            
	_reserved_words_ddl.push_back("LOCK");                                                                            
	_reserved_words_ddl.push_back("LONG");                                                                            
	_reserved_words_ddl.push_back("MAXEXTENTS");                                                                      
	_reserved_words_ddl.push_back("MINUS");                                                                           
	_reserved_words_ddl.push_back("MLSLABEL");                                                                        
	_reserved_words_ddl.push_back("MODE");                                                                            
	_reserved_words_ddl.push_back("MODIFY");                                                                          
	_reserved_words_ddl.push_back("NOAUDIT");                                                                         
	_reserved_words_ddl.push_back("NOCOMPRESS");                                                                      
	_reserved_words_ddl.push_back("NOT");                                                                             
	_reserved_words_ddl.push_back("NOWAIT");                                                                          
	_reserved_words_ddl.push_back("NULL");                                                                            
	_reserved_words_ddl.push_back("NUMBER");                                                                          
	_reserved_words_ddl.push_back("OF");                                                                              
	_reserved_words_ddl.push_back("OFFLINE");                                                                         
	_reserved_words_ddl.push_back("ON");                                                                              
	_reserved_words_ddl.push_back("ONLINE");                                                                          
	_reserved_words_ddl.push_back("OPTION");                                                                          
	_reserved_words_ddl.push_back("OR");                                                                              
	_reserved_words_ddl.push_back("ORDER");                                                                           
	_reserved_words_ddl.push_back("PCTFREE");                                                                         
	_reserved_words_ddl.push_back("PRIOR");                                                                           
	_reserved_words_ddl.push_back("PRIVILEGES");                                                                      
	_reserved_words_ddl.push_back("PUBLIC");                                                                          
	_reserved_words_ddl.push_back("RAW");                                                                             
	_reserved_words_ddl.push_back("RENAME");                                                                          
	_reserved_words_ddl.push_back("RESOURCE");                                                                        
	_reserved_words_ddl.push_back("REVOKE");                                                                          
	_reserved_words_ddl.push_back("ROW");                                                                             
	_reserved_words_ddl.push_back("ROWID");                                                                           
	_reserved_words_ddl.push_back("ROWNUM");                                                                          
	_reserved_words_ddl.push_back("ROWS");                                                                            
	_reserved_words_ddl.push_back("SELECT");                                                                          
	_reserved_words_ddl.push_back("SESSION");                                                                         
	_reserved_words_ddl.push_back("SET");                                                                             
	_reserved_words_ddl.push_back("SHARE");                                                                           
	_reserved_words_ddl.push_back("SIZE");                                                                            
	_reserved_words_ddl.push_back("SMALLINT");                                                                        
	_reserved_words_ddl.push_back("START");                                                                           
	_reserved_words_ddl.push_back("SUCCESSFUL");                                                                      
	_reserved_words_ddl.push_back("SYNONYM");                                                                         
	_reserved_words_ddl.push_back("SYSDATE");                                                                         
	_reserved_words_ddl.push_back("TABLE");                                                                           
	_reserved_words_ddl.push_back("THEN");                                                                            
	_reserved_words_ddl.push_back("TO");                                                                              
	_reserved_words_ddl.push_back("TRIGGER");                                                                         
	_reserved_words_ddl.push_back("UID");                                                                             
	_reserved_words_ddl.push_back("UNION");                                                                           
	_reserved_words_ddl.push_back("UNIQUE");                                                                          
	_reserved_words_ddl.push_back("UPDATE");                                                                          
	_reserved_words_ddl.push_back("USER");                                                                            
	_reserved_words_ddl.push_back("VALIDATE");                                                                        
	_reserved_words_ddl.push_back("VALUES");                                                                          
	_reserved_words_ddl.push_back("VARCHAR");                                                                         
	_reserved_words_ddl.push_back("VARCHAR2");                                                                        
	_reserved_words_ddl.push_back("VIEW");                                                                            
	_reserved_words_ddl.push_back("WHENEVER");                                                                        
	_reserved_words_ddl.push_back("WHERE");                                                                           
	_reserved_words_ddl.push_back("WITH");          

	return 0;
}

// Initialize the bulk copy from one database into another
int SqlOciApi::InitBulkTransfer(const char *table, size_t col_count, size_t allocated_array_rows, SqlCol *s_cols, SqlCol ** /*t_cols*/)
{
	TRACE("OCI InitBulkTransfer() Entered");
	TRACE_P("Target table: %s", table);

	// Allocate statement handle
	int rc = _ociHandleAlloc((dvoid*)_envhp, (dvoid**)&_stmtp_insert, OCI_HTYPE_STMT, 0, NULL);

	std::string insert = "INSERT INTO ";
	insert += table;
	insert += " VALUES (";

	char c[11]; *c = '\x0';

	// Add parameter markers
	for(int i = 0; i < col_count; i++)
	{
		sprintf(c, "%d", i + 1);

		insert += (i > 0) ? ", :c" : ":c";
		insert += c;
	}

	insert += ")";

	// Prepare the insert
	rc = _ociStmtPrepare(_stmtp_insert, _errhp, (text*)insert.c_str(), (ub4)insert.length(), OCI_NTV_SYNTAX, OCI_DEFAULT);

	if(rc != 0)
	{
		SetError();
		_ociHandleFree(_stmtp_insert, OCI_HTYPE_STMT);

		_stmtp_insert = NULL;
		TRACE("OCI InitBulkTransfer() Left with Error");
		return -1;
	}

	_ins_cols_count = col_count;
	_ins_allocated_rows = allocated_array_rows;
	_ins_all_rows_inserted = 0;

	_ins_cols = new SqlCol[col_count];

	// Bind columns (allocate indicators even if the source data not bound, NULL will be inserted)
	for(int i = 0; i < col_count; i++)
	{
		short *ind = NULL;

		// Allocate NULL indicators (both Sybase and Oracle use 2-byte integers)
		if(_source_api_type != SQLDATA_SYBASE)
		{
			_ins_cols[i]._ind2 = new short[_ins_allocated_rows];
			ind = _ins_cols[i]._ind2;
		}
		else
			ind = s_cols[i]._ind2;

		// Allocate length indicators
		_ins_cols[i]._len_ind2 = new short[_ins_allocated_rows];
		
		// Bind directly by default
		void *data = s_cols[i]._data;

		_ins_cols[i]._fetch_len = s_cols[i]._fetch_len;
		_ins_cols[i]._lob = s_cols[i]._lob;
		_ins_cols[i]._nchar = s_cols[i]._nchar;

        // Forward all Oracle data types
        if(_source_api_type == SQLDATA_ORACLE) 
            _ins_cols[i]._native_dt = s_cols[i]._native_fetch_dt;
        else
		// CHAR
		if( // ODBC SQL_LONGVARCHAR (CLOB) is read as SQL_C_CHAR, so skip it
			s_cols[i]._lob == false &&
			// SQL Server, Informix, DB2 and Sybase ASA types fetched as CHAR
			((_source_api_type == SQLDATA_SQL_SERVER || _source_api_type == SQLDATA_INFORMIX ||
				_source_api_type == SQLDATA_DB2 || _source_api_type == SQLDATA_ASA)
				&& (s_cols[i]._native_fetch_dt == SQL_C_CHAR || s_cols[i]._native_fetch_dt == SQL_C_WCHAR)) ||
			// MySQL data types bound to string except TEXT and BLOB
			(_source_api_type == SQLDATA_MYSQL && s_cols[i]._lob == false))
		{
			// Bind without terminating \x0, it fetched if SQL_C_CHAR but not included to fetched length 
			_ins_cols[i]._native_dt = SQLT_AFC;
 		}
		else
		// VARBINARY
		if( // ODBC SQL_LONGVARBINARY (BLOB) is read as SQL_C_BINARY, so skip it
			s_cols[i]._lob == false &&
			// SQL Server, Informix, DB2 and Sybase ASA types fetched as BINARY
			((_source_api_type == SQLDATA_SQL_SERVER || _source_api_type == SQLDATA_INFORMIX ||
				_source_api_type == SQLDATA_DB2 || _source_api_type == SQLDATA_ASA)
				&& s_cols[i]._native_fetch_dt == SQL_C_BINARY) ||
				// Sybase ASE BINARY
				(_source_api_type == SQLDATA_SYBASE && s_cols[i]._native_fetch_dt == CS_BINARY_TYPE))
		{
			_ins_cols[i]._native_dt = SQLT_BIN;
 		}
		else
		// Sybase CHAR and VARCHAR
		if(_source_api_type == SQLDATA_SYBASE && (s_cols[i]._native_fetch_dt == CS_CHAR_TYPE || s_cols[i]._native_fetch_dt == CS_LONGCHAR_TYPE))
		{
			// Bind without terminating NULL (it required in case of SQLT_STR)
			_ins_cols[i]._native_dt = SQLT_AFC;
 		}
		else
		// Sybase UNICHAR 
		if(_source_api_type == SQLDATA_SYBASE && s_cols[i]._native_fetch_dt == CS_UNICHAR_TYPE)
		{
			// Bind without terminating NULL (it required in case of SQLT_STR)
			_ins_cols[i]._native_dt = SQLT_AFC;
 		}
		else
		// INTEGER
		if(
			((_source_api_type == SQLDATA_SQL_SERVER || _source_api_type == SQLDATA_DB2 ||
				_source_api_type == SQLDATA_INFORMIX || _source_api_type == SQLDATA_ASA)
					&& s_cols[i]._native_fetch_dt == SQL_C_LONG) ||
			(_source_api_type == SQLDATA_SYBASE	&& s_cols[i]._native_fetch_dt == CS_INT_TYPE))
		{
			_ins_cols[i]._native_dt = SQLT_INT;
		}
		else
		// SQL Server DATETIME
		// Informix DATETIME bound to timestamp struct to support all unit ranges (HOUR TO SECOND i.e)
		// Sybase ASA DATETIME with fraction
		if((_source_api_type == SQLDATA_SQL_SERVER || _source_api_type == SQLDATA_DB2 || 
			_source_api_type == SQLDATA_INFORMIX || _source_api_type == SQLDATA_ASA) &&
				s_cols[i]._native_fetch_dt == SQL_TYPE_TIMESTAMP) 
		{
			// Bind to string with full format 'YYYY-MM-DD HH:MI:SS.FFFFFF'
			// Terminated by '\x0' to align multirow buffers
			_ins_cols[i]._native_dt = SQLT_STR;
			_ins_cols[i]._fetch_len = 26 + 1;

			// Allocate data for the timestamp string (conversion will happen on transfer)
			// Space for terminating 0 is required in the buffer for sprintf function
			_ins_cols[i]._data = new char[_ins_cols[i]._fetch_len * _ins_allocated_rows];
			data = _ins_cols[i]._data; 
		}
		else
		// Informix TEXT, CLOB, BYTE and BLOB (fetched as SQL_C_CHAR/SQL_C_BINARY)
		// MySQL TEXT and BLOB data types; Sybase ASA LONG VARCHAR
		if((_source_api_type == SQLDATA_INFORMIX || _source_api_type == SQLDATA_MYSQL || 
			_source_api_type == SQLDATA_ASA) && s_cols[i]._lob == true)
		{
			_ins_cols[i]._native_dt = SQLT_CLOB;
			ub1 lobtype = OCI_TEMP_CLOB;

			// Check for BLOB type
			if((_source_api_type == SQLDATA_INFORMIX &&
				 (s_cols[i]._native_dt == SQL_LONGVARBINARY || s_cols[i]._native_dt == -102)) ||
				// Sybase ASA LONG VARBINARY
				(_source_api_type == SQLDATA_ASA && s_cols[i]._native_dt == SQL_LONGVARBINARY) ||
				// MySQL BLOB, LONGBLOB
				(_source_api_type == SQLDATA_MYSQL && s_cols[i]._binary == true))
			{
				_ins_cols[i]._native_dt = SQLT_BLOB;
				lobtype = OCI_TEMP_BLOB;
			}

			_ins_cols[i]._fetch_len = 0;

			// Use data field to store pointer to the LOB locator
			rc = _ociDescriptorAlloc(_envhp, (void**)&_ins_cols[i]._data, OCI_DTYPE_LOB, 0, NULL);

			// Temporary LOB for inserting data
			rc = _ociLobCreateTemporary(_svchp, _errhp, (OCILobLocator*)_ins_cols[i]._data, 0, SQLCS_IMPLICIT, 
							lobtype, OCI_ATTR_NOCACHE, OCI_DURATION_SESSION);

			data = &_ins_cols[i]._data;
		}
		else
		// Sybase INITEXT UTF-16
		if(_source_api_type == SQLDATA_SYBASE && s_cols[i]._native_fetch_dt == CS_UNITEXT_TYPE)
		{
			_ins_cols[i]._native_dt = SQLT_CLOB;
			_ins_cols[i]._fetch_len = 0;
			_ins_cols[i]._lob = true;

			// Use data field to store pointer to the LOB locator
			rc = _ociDescriptorAlloc(_envhp, (void**)&_ins_cols[i]._data, OCI_DTYPE_LOB, 0, NULL);

			// Temporary LOB for inserting data
			rc = _ociLobCreateTemporary(_svchp, _errhp, (OCILobLocator*)_ins_cols[i]._data, 0, SQLCS_NCHAR, 
					OCI_TEMP_CLOB, OCI_ATTR_NOCACHE, OCI_DURATION_SESSION);

			data = &_ins_cols[i]._data;
 		}

		OCIBind *bindpp = NULL;

		// Bind the column
		rc = _ociBindByPos(_stmtp_insert, &bindpp, _errhp, (ub4)(i + 1), data, (sb4)_ins_cols[i]._fetch_len, 
				(ub2)_ins_cols[i]._native_dt, ind, (ub2*)_ins_cols[i]._len_ind2, NULL, 0, NULL, OCI_DEFAULT);

		// Data extracted in UTF-16
		if(_ins_cols[i]._nchar)
		{
			ub1 cform = SQLCS_NCHAR;
			ub2 csid = OCI_UTF16ID;

			rc = _ociAttrSet(bindpp, OCI_HTYPE_BIND, (void*)&cform, 0, OCI_ATTR_CHARSET_FORM, _errhp);
			rc = _ociAttrSet(bindpp, OCI_HTYPE_BIND, (void *)&csid, 0, OCI_ATTR_CHARSET_ID, _errhp);
		}

		if(rc == -1)
		{
			SetError();
			_ociHandleFree(_stmtp_insert, OCI_HTYPE_STMT);

			delete _ins_cols[i]._ind2;
			delete _ins_cols[i]._len_ind2;

			_ins_cols[i]._ind2 = NULL;
			_ins_cols[i]._len_ind2 = NULL;

			_stmtp_insert = NULL;
			TRACE("OCI InitBulkTransfer() Left with Error");
			return -1;
		}
	}

	TRACE("OCI InitBulkTransfer() Left");
	return rc;
}

// Transfer rows between databases
int SqlOciApi::TransferRows(SqlCol *s_cols, int rows_fetched, int *rows_written, size_t *bytes_written,
							size_t *time_spent)
{
	size_t bytes = 0;
	int rc = 0;

	if(rows_fetched <= 0)
		return -1;

	size_t start = GetTickCount();

	// Prepare buffers and calculate data size
	for(int i = 0; i < rows_fetched; i++)
	{
		for(int k = 0; k < _ins_cols_count; k++)
		{
			if(_source_api_type == SQLDATA_DB2 || _source_api_type == SQLDATA_SQL_SERVER || 
				_source_api_type == SQLDATA_INFORMIX || _source_api_type == SQLDATA_MYSQL ||
				_source_api_type == SQLDATA_ASA)
			{
				size_t ind = (size_t)-1;

				if(s_cols[k].ind != NULL)
					ind = s_cols[k].ind[i];

#if defined(WIN64)
				// DB2 11 64-bit CLI driver still writes indicators to 4-byte array
				if(_source_api_type == SQLDATA_DB2 /*&& s_cols[k].ind[0] & 0xFFFFFFFF00000000*/)
					ind = (size_t)((int*)(s_cols[k].ind))[i];
#endif
				// Copy indicators and calculate size for non-NULL values
				if(ind != -1)
				{
					_ins_cols[k]._ind2[i] = 0;

					// Source indicator can contain value larger than fetch size in case if data truncated
					// warning risen (Informix when LOB fecthed as VARCHAR)
					if(ind > s_cols[k]._fetch_len)
						_ins_cols[k]._len_ind2[i] = (short)s_cols[k]._fetch_len;
					else
						_ins_cols[k]._len_ind2[i] = (short)ind;

					bytes += _ins_cols[k]._len_ind2[i];
				}
				else
				{
					_ins_cols[k]._ind2[i] = -1;
					_ins_cols[k]._len_ind2[i] = 0;
				}
			}
            else
			if(_source_api_type == SQLDATA_ORACLE && s_cols[k]._ind2 != NULL)
			{
				if(s_cols[k]._ind2[i] != -1)
                {
                    _ins_cols[k]._len_ind2[i] = s_cols[k]._len_ind2[i];
                    bytes += s_cols[k]._len_ind2[i];
                }
			}
			else
			if(_source_api_type == SQLDATA_SYBASE)
			{
				// Copy length indicators and calculate size for non-NULL values
				if(s_cols[k]._ind2[i] != -1)
				{
					_ins_cols[k]._len_ind2[i] = (short)s_cols[k]._len_ind4[i];
					bytes += s_cols[k]._len_ind4[i];
				}
			}

			// SQL Server, Informix and Sybase ASA DATETIME; DB2 TIMESTAMP bound to struct
			if((_source_api_type == SQLDATA_SQL_SERVER ||_source_api_type == SQLDATA_DB2 ||
				_source_api_type == SQLDATA_INFORMIX || _source_api_type == SQLDATA_ASA) &&
					s_cols[k]._native_fetch_dt == SQL_TYPE_TIMESTAMP &&	_ins_cols[k]._ind2[i] != -1) 
			{
				size_t offset = sizeof(SQL_TIMESTAMP_STRUCT) * i;
				int ins_len = 26 + 1;

				// Informix returns '1200-01-01 11:11:00.0' for 11:11 (HOUR TO MINUTE)
				SQL_TIMESTAMP_STRUCT *ts = (SQL_TIMESTAMP_STRUCT*)(s_cols[k]._data + offset);

				long fraction = (long)ts->fraction;

				// In ODBC, fraction is stored in nanoseconds, but now we support only microseconds
				fraction = fraction/1000;

				// Convert SQL_TIMESTAMP_STRUCT to string
				Str::SqlTs2Str((short)ts->year, (short)ts->month, (short)ts->day, (short)ts->hour, (short)ts->minute, (short)ts->second, fraction, 
					_ins_cols[k]._data + ins_len * i);

				_ins_cols[k]._ind2[i] = 0;
				_ins_cols[k]._len_ind2[i] = (short)ins_len;
			}
			// Sybase ASE DATE fetched as full 26 byte timestamp string and must be truncated to Oracle DATE length
			// otherwise Oracle returns "ORA-01830: date format picture ends before converting entire input string"
			else
			if(_source_api_type == SQLDATA_SYBASE && s_cols[k]._native_dt == CS_DATE_TYPE)
			{
				short len = _ins_cols[k]._len_ind2[i];

				if(len == 26)
					_ins_cols[k]._len_ind2[i] = 19;
			}
			else
			// LOB column
			if(_source_api_type != SQLDATA_ORACLE && s_cols[k]._lob == true && _ins_cols[k]._ind2[i] != -1)
			{
				OCILobLocator *loc = (OCILobLocator*)_ins_cols[k]._data;

				// The previous LOB locator value must be truncated
				rc = _ociLobTrim(_svchp, _errhp, loc, 0);
				
				size_t lob_size = 0;

				bool more = true;

				// Get LOB in chunks
				while(more)
				{
					ub4 ampt = (ub4)s_cols[k].ind[i];
					ub1 piece = OCI_ONE_PIECE;

					// Check if the buffer fully filled (buffer is x0 terminated for CLOBs)
					if(((_source_api_type == SQLDATA_INFORMIX &&
						(s_cols[k]._native_dt == SQL_LONGVARCHAR || s_cols[k]._native_dt == -103) ||
						// Sybase ASA LONG VARCHAR
						(_source_api_type == SQLDATA_ASA && s_cols[k]._native_dt == SQL_LONGVARCHAR))
						&& (size_t)s_cols[k].ind[i] == s_cols[k]._fetch_len - 1) ||
					  // BLOB are not reminated with 0x
					  ((_source_api_type == SQLDATA_INFORMIX &&
						(s_cols[k]._native_dt == SQL_LONGVARBINARY || s_cols[k]._native_dt == -102) ||
						// Sybase ASA LONG VARBINARY
						(_source_api_type == SQLDATA_ASA && s_cols[k]._native_dt == SQL_LONGVARBINARY))
						&& (size_t)s_cols[k].ind[i] == s_cols[k]._fetch_len))
					{
						piece = (lob_size == 0) ? (ub1)OCI_FIRST_PIECE : (ub1)OCI_NEXT_PIECE;
						ampt = 0;
					}
					else
					// MySQL API returns LOB in one piece
					if(_source_api_type == SQLDATA_MYSQL && s_cols[k]._native_dt == MYSQL_TYPE_BLOB)
					{
						piece = OCI_ONE_PIECE;
						ampt = (ub4)s_cols[k].ind[i];
					}
					else
						piece = (lob_size == 0) ? (ub1)OCI_ONE_PIECE : (ub1)OCI_LAST_PIECE;

					// Write LOB chunk
					rc = _ociLobWrite(_svchp, _errhp, loc, &ampt, (ub4)(lob_size + 1), s_cols[k]._data, 
							(ub4)s_cols[k].ind[i], piece, NULL, NULL, 0, SQLCS_IMPLICIT);

					lob_size += s_cols[k].ind[i];

					// Get next chunk
					if(piece == OCI_FIRST_PIECE || piece == OCI_NEXT_PIECE)
					{
						rc = _source_api_provider->GetLobPart(0, (size_t)k, NULL, 0, NULL);

						if(rc != -1)
							continue;
					}

					more = false;
					break;
				}

				bytes += lob_size;
			}
			else
			// Correct the size for UTF-16 data since buffer in bytes while Oracle OCI requires size in characters
			if(_ins_cols[k]._nchar)
			{
				short len = _ins_cols[k]._len_ind2[i];

				if(len > 2 && len % 2 == 0)
					_ins_cols[k]._len_ind2[i] = len/2;
			}
		}
	}

	// Insert rows
	rc = _ociStmtExecute(_svchp, _stmtp_insert, _errhp, (ub4)rows_fetched, 0, NULL, NULL, OCI_DEFAULT);

	if(rc == -1)
	{
		SetError();
		return -1;
	}

	_ins_all_rows_inserted += rows_fetched;

	// Commit the batch
	if(_ins_allocated_rows > 1)
		rc = _ociTransCommit(_svchp, _errhp, OCI_DEFAULT);
	else
	// Commit table with LOB every 100 rows
	if(_ins_all_rows_inserted % 100 == 0)
		rc = _ociTransCommit(_svchp, _errhp, OCI_DEFAULT);

	if(rows_written != NULL)
		*rows_written = rows_fetched;

	if(bytes_written != NULL)
		*bytes_written = bytes;

	if(time_spent)
		*time_spent = GetTickCount() - start;

	return rc;
}

// Complete bulk transfer
int SqlOciApi::CloseBulkTransfer()
{
	int rc = 0;

	// Commit table with LOB every 100 rows
	if(_ins_allocated_rows == 1)
		rc = _ociTransCommit(_svchp, _errhp, OCI_DEFAULT);

	// Close the statement
	rc = _ociHandleFree(_stmtp_insert, OCI_HTYPE_STMT);

	_stmtp_insert = NULL;

	// Delete indicators and descriptors
	for(int i = 0; i < _ins_cols_count; i++)
	{
		// Free temporary LOB and locator
		if(_ins_cols[i]._lob == true)
		{
			rc = _ociLobFreeTemporary(_svchp, _errhp, (OCILobLocator*)_ins_cols[i]._data);
			rc = _ociDescriptorFree(_ins_cols[i]._data, OCI_DTYPE_LOB); 
		}

		if(_ins_cols[i]._lob == false)
			delete [] _ins_cols[i]._data;

		delete [] _ins_cols[i]._ind2;
		delete [] _ins_cols[i]._len_ind2;
	}

	_ins_cols_count = 0;
	_ins_allocated_rows = 0;
	_ins_all_rows_inserted = 0;
	
	delete [] _ins_cols;

	return rc;
}

// Get the maximum size of a character in the client character set in bytes (4 for UTF-8)
int SqlOciApi::GetCharMaxSizeInBytes()
{
	// AL32UTF8
	if(_charset_id == 873)
		return 4;

	return 1;
}

// Drop the table
int SqlOciApi::DropTable(const char* table, size_t *time_spent, std::string &drop_stmt)
{
	// Although foreign keys are removed before transfer, CASCADE CONSTRAINTS is required 
	// if non-selected tables have references to this table
	drop_stmt = "DROP TABLE ";
	drop_stmt += table;
	drop_stmt += " CASCADE CONSTRAINTS";

	int rc = ExecuteNonQuery(drop_stmt.c_str(), time_spent);

	return rc;
}

// Remove foreign key constraints referencing to the parent table
// Foreign keys are removed before transfer as CASCADE CONSTRAINTS causes 'resource busy' errors in concurrent environment

int SqlOciApi::DropReferences(const char* table, size_t *time_spent)
{
	if(table == NULL)
		return -1;

	size_t start = (time_spent != NULL) ? Os::GetTickCount() : 0;

	std::string schema;
	std::string object;

	SplitQualifiedName(table, schema, object);

	int rc = 0;

	// Find a primary or unique constraint defined on this table
	for(std::list<SqlConstraints>::iterator i = _table_constraints.begin(); i != _table_constraints.end(); i++)
	{
		if((*i).type != 'P' && (*i).type != 'U')
			continue;

		char *s = (*i).schema;
		char *t = (*i).table;
		char *c = (*i).constraint;

		if(s == NULL || t == NULL || c == NULL)
			continue;
		
		// Not this table (compare case-insensitive as objects in DDL can be in any case)
		if(_stricmp(t, object.c_str()) != 0 || _stricmp(s, schema.c_str()) != 0)
			continue;

		// Now find a foreign key referencing to this primary or unqiue constraint
		for(std::list<SqlConstraints>::iterator k = _table_constraints.begin(); k != _table_constraints.end(); k++)
		{
			if((*k).type != 'R')
				continue;

			char *fk_s = (*k).schema;
			char *fk_t = (*k).table;
			char *fk_c = (*k).constraint;
			char *pk_s = (*k).r_schema;
			char *pk_c = (*k).r_constraint;

			if(fk_s == NULL || fk_t == NULL || fk_c == NULL || pk_s == NULL || pk_c == NULL)
				continue;
		
			// Not this constraint
			if(strcmp(c, pk_c) != 0 && strcmp(s, pk_s) != 0)
				continue;

			// Generate ALTER TABLE child DROP CONSTRAINT name
			std::string drop = "ALTER TABLE ";
			drop += fk_s;
			drop += ".";
			drop += fk_t;

			drop += " DROP CONSTRAINT ";
			drop += fk_c;

			rc = ExecuteNonQuery(drop.c_str(), NULL);
		}		
	}

	if(time_spent != NULL)
		*time_spent = GetTickCount() - start;

	return rc;
}

// Remove foreign key constraints on the specified child table
int SqlOciApi::DropReferencesChild(const char* table, size_t *time_spent, int *keys)
{
	if(table == NULL)
		return -1;

	size_t start = (time_spent != NULL) ? Os::GetTickCount() : 0;
	int found = 0;

	std::string schema;
	std::string object;

	SplitQualifiedName(table, schema, object);

	int rc = 0;

	// Find a foreing key constraint defined on this table
	for(std::list<SqlConstraints>::iterator i = _table_constraints.begin(); i != _table_constraints.end(); i++)
	{
		if((*i).type != 'R')
			continue;

		char *s = (*i).schema;
		char *t = (*i).table;
		char *c = (*i).constraint;

		if(s == NULL || t == NULL || c == NULL)
			continue;
		
		// Not this table (compare case-insensitive as objects in DDL can be in any case)
		if(_stricmp(t, object.c_str()) != 0 || _stricmp(s, schema.c_str()) != 0)
			continue;

		// Generate ALTER TABLE child DROP CONSTRAINT name
		std::string drop = "ALTER TABLE ";
		drop += table;
		drop += " DROP CONSTRAINT ";
		drop += c;

		rc = ExecuteNonQuery(drop.c_str(), NULL);

		found++;
	}

	if(keys != NULL)
		*keys = found;

	if(time_spent != NULL)
		*time_spent = GetTickCount() - start;

	return rc;
}

// Get query to retrieve all available tables
void SqlOciApi::GetAllTablesQuery(std::string &table_template, std::string &query, const char *default_schema)
{
	std::string selection;

	// Build WHERE clause to select rows from catalog
	GetSelectionCriteria(table_template.c_str(), "owner", "table_name", selection, default_schema, true);

	query = "SELECT owner, table_name FROM all_tables ";
	query += selection;
}

// Set version of the connected database
void SqlOciApi::SetVersion()
{
	// Oracle 11g R2 Express returns - Oracle Database 11g Express Edition Release 11.2.0.2.0 - Beta 
	const char *query = "SELECT banner FROM v$version WHERE banner LIKE 'Oracle%'";

	size_t col_count = 0;
	size_t allocated_rows = 0;
	int rows_fetched = 0; 
	size_t time_read = 0;
	
	SqlCol *cols = NULL;

	// Open cursor 
	int rc = OpenCursor(query, 1, 0, &col_count, &allocated_rows, &rows_fetched, &cols, &time_read, true);

	// Get the value
	if(rc >= 0)
	{
		int len = -1;

		if(cols != NULL && cols[0]._ind2 != NULL && cols[0]._len_ind2 != NULL && cols[0]._ind2[0] != -1)
			len = cols[0]._len_ind2[0];

		if(len != -1 && len < 127)
			_version.assign(cols[0]._data, (size_t)len);
	}

	CloseCursor();
}

// Set error code and message for the last API call
void SqlOciApi::SetError()
{
	if(_ociErrorGet == NULL)
		return;

	_native_error_text[0] = '\x0';

	// Get error information
	_ociErrorGet(_errhp, 1, NULL, &_native_error, (text*)_native_error_text, 1024, OCI_HTYPE_ERROR);

	_native_error_text[1023] = '\x0';

	size_t len = strlen(_native_error_text);

	// OCIErrorGet returns 0A before terminating zero, remove it
	if(_native_error_text[len-1] == '\x0A')
		_native_error_text[len-1] = '\x0';

	_error = SQL_DBAPI_UNKNOWN_ERROR;
	*_error_text = '\x0';

	// ORA-00942: table or view does not exist
	// ORA-02289: sequence does not exist
	if(_native_error == 942 || _native_error == 2289)
		_error = SQL_DBAPI_NOT_EXISTS_ERROR;
}

// Find Oracle installation paths
void SqlOciApi::FindOraclePaths()
{
#ifdef WIN32
	HKEY hKey, hSubkey;

	// Oracle 11g Express contains subkey KEY_XE; Oracle 11g Enterprise KEY_OraDb11g_home1; Oracle 12 - KEY_OraDB12Home1
	// Oraclle 11g Client KEY_OraClient11g_home1
	int rc = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Oracle", 0, KEY_READ | KEY_ENUMERATE_SUB_KEYS, &hKey);

	if(rc != ERROR_SUCCESS)
		return;

	char key[1024];
	DWORD i = 0;

	bool more = true;

	// Enumerate through all keys
	while(more)
	{
		// Size modified with each call to RegEnumKeyEx
		int size = 1024;

		// Get next key
		rc = RegEnumKeyEx(hKey, i, key, (LPDWORD)&size, NULL, NULL, NULL, NULL);
 
		if(rc != ERROR_SUCCESS)
		{
			more = false;
			break;
		}

		i++;

		// Open each subkey and try to find ORACLE_HOME
		rc = RegOpenKeyEx(hKey, key, 0, KEY_READ, &hSubkey);

		if(rc != ERROR_SUCCESS)
			break;

		char value[1024];
		int value_size = 1024;

		rc = RegQueryValueEx(hSubkey, "ORACLE_HOME", NULL, NULL, (LPBYTE)value, (LPDWORD)&value_size); 

		if(rc == ERROR_SUCCESS)
		{
			// Oracle home does not include terminating '\'
			std::string loc = value;
			loc += "\\bin\\";

			_driver_paths.push_back(loc);
		}
  
		RegCloseKey(hSubkey);	
	}

	RegCloseKey(hKey);

#endif
}

