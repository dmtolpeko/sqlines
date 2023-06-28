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

#ifndef sqlines_sqlociapi_h
#define sqlines_sqlociapi_h

#if defined(WIN32) || defined(_WIN64)
#include <windows.h>
#endif

#include <oci.h>
#include "sqlapibase.h"
#include "sqldb.h"

#if defined(WIN32) || defined(_WIN64)
#define OCI_DLL						"oci.dll"
#define OCI_64BIT_DLL_LOAD_ERROR	"Loading Oracle OCI oci.dll: 64-bit library loaded, use 32-bit Oracle client or 64-bit version of SQLines Data"
#else
#define OCI_DLL						"libclntsh.so"
#endif

#define OCI_DLL_LOAD_ERROR		"Error loading Oracle " OCI_DLL ": " 

// Oracle OCI DLL Functions
typedef sword (*OCIArrayDescriptorAllocFunc)(void *, void **, ub4, ub4, size_t, void **);
typedef sword (*OCIAttrGetFunc)(void *, ub4, void *, ub4 *, ub4, OCIError *);
typedef sword (*OCIAttrSetFunc)(void *, ub4, void *, ub4, ub4, OCIError *);
typedef sword (*OCIBindByPosFunc)(OCIStmt *stmtp, OCIBind **bindpp, OCIError *errhp, ub4 position, dvoid *valuep, sb4 value_sz, ub2 dty, dvoid *indp, ub2 *alenp, ub2 *rcodep, ub4 maxarr_len, ub4 *curelep, ub4 mode);
typedef sword (*OCIDefineByPosFunc)(OCIStmt *, OCIDefine **, OCIError *, ub4, dvoid *, sb4, ub2, dvoid *, ub2 *, ub2 *, ub4);
typedef sword (*OCIDescriptorAllocFunc)(void *, void **, ub4, size_t, void **);
typedef sword (*OCIDescriptorFreeFunc)(void *, ub4);
typedef sword (*OCIEnvCreateFunc)(OCIEnv **envhpp, ub4 mode, CONST dvoid *ctxp, CONST dvoid *(*malocfp)(dvoid *ctxp, size_t size), CONST dvoid *(*ralocfp)(dvoid *ctxp, dvoid *memptr, size_t newsize), CONST void (*mfreefp)(dvoid *ctxp, dvoid *memptr), size_t xtramemsz, dvoid **usrmempp);
typedef sword (*OCIErrorGetFunc)(dvoid *, ub4, text *, sb4 *, text *, ub4, ub4);
typedef sword (*OCIHandleAllocFunc)(void *, void **, ub4, size_t, void **);
typedef sword (*OCIHandleFreeFunc)(dvoid *, ub4);
typedef sword (*OCILobCharSetFormFunc)(OCIEnv *envhp, OCIError *errhp, CONST OCILobLocator *locp, ub1 *csfrm);
typedef sword (*OCILobCopyFunc)(OCISvcCtx *, OCIError *, OCILobLocator *dst, OCILobLocator *src, ub4 amount, ub4 dst_ffset, ub4 src_offset);
typedef sword (*OCILobCreateTemporaryFunc)(OCISvcCtx *, OCIError *, OCILobLocator *, ub2 csid, ub1 csfrm, ub1 lobtype, boolean cache, OCIDuration);
typedef sword (*OCILobFreeTemporaryFunc)(OCISvcCtx *, OCIError *, OCILobLocator *);
typedef sword (*OCILobGetLength2Func)(OCISvcCtx *, OCIError *, OCILobLocator *, oraub8 *);
typedef sword (*OCILobRead2Func)(OCISvcCtx *, OCIError *, OCILobLocator *, oraub8 *, oraub8 *, oraub8, void *, oraub8, ub1, void *, OCICallbackLobRead2(cbfp)(void *, const void *, oraub8, ub1, void **, oraub8 *), ub2, ub1);
typedef sword (*OCILobTrimFunc)(OCISvcCtx *, OCIError *, OCILobLocator *, ub4 newlen);
typedef sword (*OCILobWriteFunc)(OCISvcCtx *, OCIError *, OCILobLocator *, ub4 *amtp, ub4 offset, dvoid *bufp, ub4 buflen, ub1 piece, dvoid *ctxp, OCICallbackLobWrite (cbfp)(dvoid *, dvoid *, ub4 *, ub1 *), ub2 csid, ub1 csfrm);
typedef sword (*OCILobWriteAppendFunc)(OCISvcCtx *, OCIError *, OCILobLocator *locp, ub4 *amtp, dvoid *bufp, ub4 buflen, ub1 piece, dvoid *ctxp, OCICallbackLobWrite(cbfp)(dvoid *, dvoid *, ub4 *, ub1 *), ub2 csid, ub1 csfrm);
typedef sword (*OCIParamGetFunc)(const void *hndlp, ub4, OCIError *, void **, ub4);
typedef sword (*OCIServerAttachFunc)(OCIServer *, OCIError *, const OraText *, sb4, ub4);
typedef sword (*OCIServerDetachFunc)(OCIServer *srvhp, OCIError *errhp, ub4 mode);
typedef sword (*OCISessionBeginFunc)(OCISvcCtx *, OCIError *, OCISession *, ub4, ub4);
typedef sword (*OCISessionEndFunc)(OCISvcCtx *svchp, OCIError *errhp, OCISession *usrhp, ub4 mode);
typedef sword (*OCIStmtExecuteFunc)(OCISvcCtx *, OCIStmt *, OCIError *, ub4, ub4, CONST OCISnapshot *, OCISnapshot *, ub4);
typedef sword (*OCIStmtFetch2Func)(OCIStmt *, OCIError *, ub4, ub2, sb4, ub4);
typedef sword (*OCIStmtPrepareFunc)(OCIStmt *, OCIError *, CONST text *, ub4, ub4, ub4);
typedef sword (*OCITransCommitFunc)(OCISvcCtx *svchp, OCIError *errhp, ub4 flags);

class SqlOciApi : public SqlApiBase
{
	// OCI DLL handle
#if defined(WIN32) || defined(_WIN64)
	HMODULE _oci_dll;
#else
	void* _oci_dll;
#endif

	OCIEnv *_envhp;
	OCIServer *_srvhp;
	OCISession *_authp;
	OCISvcCtx *_svchp;
	OCIError *_errhp;

	OCIStmt *_stmtp_cursor;
	OCIStmt *_stmtp_insert;

	// Connection information
	std::string _user;
	std::string _pwd;
	std::string _db;

	// Character set ID (873 - AL32UTF8)
	ub2 _charset_id;

	// Whether to bind LONG in place without using locators (used in metadata read of short LONG columns)
	size_t _bind_long_inplace;

public:
	SqlOciApi();

	// Initialize API (Load the OCI driver)
	virtual int Init();

	// Set additional information about the driver type
	virtual void SetDriverType(const char * /*info*/) {}

	// Set the connection string in the API object
	virtual void SetConnectionString(const char *conn);

	// Connect to the database
	virtual int Connect(size_t *time_spent);

	// Disconnect from the database
	virtual void Disconnect();
	// Deallocate the driver
	virtual void Deallocate();

	// Get row count for the specified object
	virtual int GetRowCount(const char *object, int *count, size_t *time_spent);

	// Execute the statement and get scalar result
	virtual int ExecuteScalar(const char *query, int *result, size_t *time_spent);

	// Execute the statement
	virtual int ExecuteNonQuery(const char *query, size_t *time_spent);

	// Open cursor and allocate buffers
	virtual int OpenCursor(const char *query, size_t buffer_rows, int buffer_memory, size_t *col_count, size_t *allocated_array_rows, 
		int *rows_fetched, SqlCol **cols, size_t *time_spent, bool catalog_query = false,
        std::list<SqlDataTypeMap> *dtmap = NULL);

	// Fetch next portion of data to allocate buffers
	virtual int Fetch(int *rows_fetched, size_t *time_spent);

	// Close the cursor and deallocate buffers
	virtual int CloseCursor();

	// Initialize the bulk copy from one database into another
	virtual int InitBulkTransfer(const char *table, size_t col_count, size_t allocated_array_rows, SqlCol *s_cols, SqlCol **t_cols);

	// Transfer rows between databases
	virtual int TransferRows(SqlCol *s_cols, int rows_fetched, int *rows_written, size_t *bytes_written,
			size_t *time_spent);

	// Specifies whether API allows to parallel reading from this API and write to another API
	virtual bool CanParallelReadWrite();

	// OCI bounds the source buffers, so the same buffers must be passed to TransferRows
	virtual bool IsDataBufferBound() { return true; }

	// Get the maximum size of a character in the client character set in bytes (4 for UTF-8)
	virtual int GetCharMaxSizeInBytes();

	// Complete bulk transfer
	virtual int CloseBulkTransfer();

	// Drop the table
	virtual int DropTable(const char* table, size_t *time_spent, std::string &drop_stmt);
	// Remove foreign key constraints referencing to the parent table
	virtual int DropReferences(const char* table, size_t *time_spent);
	// Remove foreign key constraints on the specified child table
	int DropReferencesChild(const char* table, size_t *time_spent, int *keys);

	// Get the length of LOB column in the open cursor
	virtual int GetLobLength(size_t row, size_t column, size_t *length);
	// Allocate the buffer to read the LOB value depending on character lengths
	char* GetLobBuffer(size_t row, size_t column, size_t length, size_t *alloc_len);
	void FreeLobBuffer(char *buf);
	// Get LOB content
	virtual int GetLobContent(size_t row, size_t column, void *data, size_t length, int *len_ind);
	// Get partial LOB content
	virtual int GetLobPart(size_t /*row*/, size_t /*column*/, void * /*data*/, size_t /*length*/, int * /*len_ind*/) { return -1; }

	// Get the list of available tables
	virtual int GetAvailableTables(std::string &select, std::string &exclude, std::list<std::string> &tables);

	// Read schema information
	virtual int ReadSchema(const char *select, const char *exclude, bool read_cns = true, bool read_idx = true);
	virtual int ReadSchemaForTransferTo(const char *select, const char *exclude);

	// Read specific catalog information
	virtual int ReadConstraintTable(const char *schema, const char *constraint, std::string &table);
	virtual int ReadConstraintColumns(const char *schema, const char *table, const char *constraint, std::string &cols);

	// Get a list of columns for specified primary or unique key
	virtual int GetKeyConstraintColumns(SqlConstraints &cns, std::list<std::string> &output, std::list<std::string> * = NULL);

	// Get a list of columns for specified index
	virtual int GetIndexColumns(SqlIndexes &idx, std::list<std::string> &idx_cols, std::list<std::string> &idx_sorts);

	// Get column default by table and column name
	virtual int GetColumnDefault(const char *table, const char *column, std::string &col_default);
	// Get table or column comment by table and column name
	virtual int GetComment(const char *table, const char *column, std::string &comment); 

	// Get database type and subtype
	virtual short GetType() { return SQLDATA_ORACLE; }
	virtual short GetSubType() { return 0; }

public:
	// Find Oracle installation paths
	void FindOraclePaths();

	// Metadata queries
	static void GetAllTablesQuery(std::string &table_template, std::string &query, const char *default_schema);

	// Get fatures supported by the provider
	virtual bool IsIntegerBulkBindSupported() { return true; }
	virtual bool IsSmallintBulkBindSupported() { return false; }

private:
	int ReadTableColumns(std::string &selection);
	int ReadTableConstraints(std::string &selection, const char *types);
	int ReadConstraintColumns(std::string &selection);
	int ReadComments(std::string &selection);
	int ReadIndexes(std::string &selection);
	int ReadIndColumns(std::string &selection);
	int ReadIndExpressions(std::string &selection);
	int ReadSequences(std::string &selection);
	int ReadReservedWords();

    int InitSession();
	// Set session attributes
	int SetSession();
	// Set version of the connected database
	void SetVersion();

	// Oracle OCI Function pointers
	OCIArrayDescriptorAllocFunc _ociArrayDescriptorAlloc;
	OCIAttrGetFunc _ociAttrGet;
	OCIAttrSetFunc _ociAttrSet;
	OCIBindByPosFunc _ociBindByPos;
	OCIDescriptorAllocFunc _ociDescriptorAlloc;
	OCIDescriptorFreeFunc _ociDescriptorFree;
	OCIDefineByPosFunc _ociDefineByPos;
	OCIEnvCreateFunc _ociEnvCreate;
	OCIErrorGetFunc _ociErrorGet;
	OCIStmtFetch2Func _ociStmtFetch2;
	OCIHandleAllocFunc _ociHandleAlloc;
	OCIHandleFreeFunc _ociHandleFree;
	OCILobCharSetFormFunc _ociLobCharSetForm;
	OCILobCopyFunc _ociLobCopy;
	OCILobCreateTemporaryFunc _ociLobCreateTemporary;
	OCILobFreeTemporaryFunc _ociLobFreeTemporary;
	OCILobGetLength2Func _ociLobGetLength2;
	OCILobRead2Func _ociLobRead2;
	OCILobTrimFunc _ociLobTrim;
	OCILobWriteFunc _ociLobWrite;
	OCILobWriteAppendFunc _ociLobWriteAppend;
	OCIParamGetFunc _ociParamGet;
	OCIServerAttachFunc _ociServerAttach;
	OCIServerDetachFunc _ociServerDetach;
	OCISessionBeginFunc _ociSessionBegin;
	OCISessionEndFunc _ociSessionEnd;
	OCIStmtExecuteFunc _ociStmtExecute;
	OCIStmtPrepareFunc _ociStmtPrepare;
	OCITransCommitFunc _ociTransCommit;

	// Set error code and message for the last API call
	void SetError();
};

#endif // sqlines_sqlociapi_h