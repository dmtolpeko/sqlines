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

// SqlApiBase Abstract database API class

#ifndef sqlines_sqlapibase_h
#define sqlines_sqlapibase_h

#include <string>
#include <list>
#include "parameters.h"
#include "applog.h"
#include "file.h"

// Error codes
#define SQL_DBAPI_UNKNOWN_ERROR			    -1
#define SQL_DBAPI_NOT_EXISTS_ERROR			-2

// ON UPDATE and ON DELETE actions for foreign keys
#define FK_ACTION_RESTRICT		1
#define FK_ACTION_NO_ACTION		2
#define FK_ACTION_CASCADE		3
#define FK_ACTION_SET_NULL		4
#define FK_ACTION_SET_DEFAULT	5

#define TRACE(mess) { if(_trace && _log != NULL) _log->Trace(mess); }
#define TRACE_P(mess, ...) { if(_trace && _log != NULL) _log->Trace(mess, ##__VA_ARGS__); }
#define TRACE_S(obj, mess) { if(obj->_trace && obj->_log != NULL) obj->_log->Trace(mess); }

#define TRACE_DMP_INIT(file) { if(_trace_data && _log != NULL) { File::Truncate(file); _trace_data_file = file; } } 
#define TRACE_DMP(data, len) { if(_trace_data) File::Append(_trace_data_file.c_str(), data, len); } 

#define LOG(mess) { if(_log != NULL) { _log->Log(mess);  if(_trace) _log->Trace(mess); } }
#define LOG_P(mess, ...) { if(_log != NULL) { _log->Log(mess, ##__VA_ARGS__);  if(_trace) _log->Trace(mess, ##__VA_ARGS__); } }

extern const char *g_spec_symbols;

// Column describtion for cursors
struct SqlCol
{
	// Column name
	char _name[256];
	// Target column name 
	char _t_name[256];

	// Native data type code
	int _native_dt;
	// Native fetch data type code (to which retrieve is bound)
	int _native_fetch_dt;

	// Column length in bytes for string and binary strings
	size_t _len;
	// Column length in bytes for bound data (one element in array)
	size_t _fetch_len;

	// Precision and scale for numeric data types (scale also specifies fraction for TIMESTAMP)
	int _precision;
	int _scale;	

	// Column contains binary data (MySQL)
	bool _binary;
	// Data fetched in UTF-16
	bool _nchar;
	
    // Column data type name
    std::string _datatype_name;
    std::string _t_datatype_name;
	
	// Column data (can be array)
	char *_data;

	// 2-byte NULL indicators for Oracle OCI and Sybase CT-Lib
	short *_ind2;

	// NULL and length indicator for ODBC (4-byte for 32-bit Windows, and 8-byte for 64-bit Windows)
	// size_t is 32-bit on 32-bit Windows and 64-bit on 64-bit Windows
	size_t *ind;

	// 2-byte length indicators for Oracle OCI
	short *_len_ind2;
	// 4-byte length indicator is required by Sybase CT-Lib
	int *_len_ind4;

	// Column can store NULLs
	bool _nullable;

	// Column is a large object
	bool _lob;
	// For ODBC, the fetch status for last LOB fetch (returned by SQLGetData)
	int _lob_fetch_status;

	// For data validation, the total number of rows where the column values are different
	int _diff_rows;

	SqlCol() 
	{ 
		*_name = '\x0'; *_t_name = '\x0';
		_len = 0; _fetch_len = 0; _precision = 0; _scale = 0; _binary = false; _nchar = false;
		_native_dt = 0; _native_fetch_dt = 0; 
		_data = NULL; _ind2 = NULL; /*ind4 = NULL;*/ ind = NULL; _len_ind2 = NULL; _len_ind4 = NULL;
		_nullable = true; _lob = false; _lob_fetch_status = 0;
		_diff_rows = 0;
	}
};

// Column metadata information from the catalog
struct SqlColMeta
{
	// Schema, table and column names
	char *schema;
	char *table;
	char *column;
	
	// Column number within the table 
	int num;

	// Data type as string in Oracle
	char *data_type;
	// Data type as code in Informix (0 is CHAR)
	int data_type_code; 

	// Default clause
	bool no_default;
	// String literal must be quoted
	char *default_value;

	// Default value type (Informix) L - string or number literal
	char default_type;

    // Column can store NULLs
	bool nullable;

    // Column belongs to primary key
    bool pk_column;

	// Identity (auto_increment, serial) column
	bool identity;
	int id_start;
	int id_inc;
	int id_next;

	// Table ID for quick search and joins (Informix, Sybase ASA)
	int tabid;

	SqlColMeta()
	{
		schema = NULL; table = NULL; column = NULL; data_type = NULL;
		data_type_code = 0; num = 0;
		no_default = true; default_value = NULL; default_type = '\x0';
        nullable = true;
        pk_column = false;
		identity = false; id_start = 0; id_inc = 0; id_next = 0;
		tabid = 0;
	}

	~SqlColMeta()
	{
		delete schema;	delete table; delete column; delete data_type; delete default_value; 
	}
};

// Column name and data type mapping
struct SqlColMap
{
	// Schema name
	std::string schema;
	// Table name
	std::string table;
	
	// Source column name
	std::string name;
	// Target column name
	std::string t_name;

	// Target data type with length attributes 
	std::string t_type;
};

// Global data type mapping
struct SqlDataTypeMap
{
	// Source data type name
	std::string name;

    // Source data type range
    int len_min;
    int len_max;
    int scale_min;
    int scale_max;

	// Target data type name
	std::string t_name;
};

// Table constraints metadata information from the catalog
struct SqlConstraints
{
	// Schema, table and constraint names
	char *schema;
	char *table;
	char *constraint;

	// Constraint type P for primary key, C for check, R for foreign key, U for unique
	char type;

	// CHECK constraint condition
	char *condition;

	// Constraint ID (Informix, Sybase ASA)
	int cnsid;
	// Table ID for quick search and joins (Informix, Sybase ASA, Sybase ASE)
	int tabid;

	// Index name to refer to constraint columns (Informix)
	char *idxname;
	// Index ID (Sybase ASA, Sybase ASE) 
	int idxid;

	// For foreign keys, reference to the primary key or unique constraint (Oracle)
	char *r_schema;
	char *r_constraint;
	
	// For foreign keys, primary key constraint ID (Informix, Sybase ASA)
	int r_cnsid;

	// Foreign key ID (Sybase ASA)
	int fkid;

	// Primary key table for foreign keys (Sybase ASE)
	char *pk_schema;
	char *pk_table;

	// ON UPDATE and ON DELETE actions for foreign keys
	char fk_on_update;
	char fk_on_delete;

	SqlConstraints()
	{
		schema = NULL; table = NULL; constraint = NULL; condition = NULL; 
		r_schema = NULL; r_constraint = NULL;
		type = '\x0';
		cnsid = 0; tabid = 0; idxid = 0; r_cnsid = 0; fkid = 0;
		idxname = NULL; 
		pk_schema = NULL; pk_table = NULL;
		fk_on_update = 0; fk_on_delete = 0;
	}

	~SqlConstraints()
	{
		delete schema; delete table; delete constraint; 
		delete condition; delete r_schema; delete r_constraint;
		delete idxname;
		delete pk_schema; delete pk_table;
	}
};

// Constraint columns information from the catalog
struct SqlConsColumns
{
	// Schema, table, constraint and column names
	char *schema;
	char *table;
	char *constraint;
	char *column;

	// Constraint ID (Sybase ASE, Sybase ASA)
	int cnsid;
	// Table ID (Sybase ASE, Sybase ASA)
	int tabid;

	// Corresponding primary key column for foreign key (Sybase ASE)
	char *pk_column;

	SqlConsColumns()
	{
		schema = NULL; table = NULL; constraint = NULL; column = NULL; pk_column = NULL;
		cnsid = 0; tabid = 0;
	}

	~SqlConsColumns()
	{
		delete schema;	delete table; delete constraint; delete column; delete pk_column;
	}
};

// Indexes metadata information from the catalog
struct SqlIndexes
{
	// Schema and index names
	char *schema;
	char *index;

	bool unique;

	// Table owner and name
	char *t_schema;
	char *t_name;

	// Table ID (for SQL Server, Informix, Sybase ASA)
	int tabid;
	// Index ID (SQL Server, Sybase ASA)
	int idxid;

	SqlIndexes()
	{
		schema = NULL; index = NULL; t_schema = NULL; t_name = NULL; 
		tabid = 0; idxid = 0; unique = false;
	}

	~SqlIndexes()
	{
		delete schema;	delete index; delete t_schema; delete t_name;
	}
};

// Index columns information from the catalog
struct SqlIndColumns
{
	// Schema, index and column names
	char *schema;
	char *index;
	char *column;

	// Table ID (Informix, Sybase ASA)
	int tabid;
	// Index ID (Sybase ASA)
	int idxid;

	bool asc;

	SqlIndColumns()
	{
		schema = NULL; index = NULL; column = NULL; 
		tabid = 0; idxid = 0;
		asc = true;
	}

	~SqlIndColumns()
	{
		delete schema;	delete index; delete column;
	}
};

// Index expressions for function-based indexes from the catalog
struct SqlIndExp
{
	// Schema, index and column names
	char *schema;
	char *index;
	char *expression;

	SqlIndExp()
	{
		schema = NULL; index = NULL; expression = NULL; 
	}

	~SqlIndExp()
	{
		delete schema;	delete index; delete expression;
	}
};

// Table and column comments from the catalog
struct SqlComments
{
	// Schema, table and constraint names
	char *schema;
	char *table;
	char *column;
	char *comment;

	// Comment type 'C' for columns, 'T' for tables, and 'V' for views
	char type;

	SqlComments()
	{
		schema = NULL; table = NULL; column = NULL; comment = NULL;
		type = '\x0';
	}

	~SqlComments()
	{
		delete schema;	delete table; delete column; delete comment;
	}
};

// Sequences
struct SqlSequences
{
	// Schema and name of the sequence
	char *schema;
	char *name;

	// Sequence options in string form
	char *min_str;
	char *max_str;
	char *inc_str;
	char *cur_str;
	char *cache_size_str;

	bool cycle; 

	bool no_max;

	SqlSequences()
	{
		schema = NULL; name = NULL; min_str = NULL; max_str = NULL; inc_str = NULL; cur_str = NULL; cache_size_str = NULL; 
		cycle = false; no_max = false;
	}

	~SqlSequences()
	{
		delete schema; delete name; delete min_str; delete max_str; delete inc_str; delete cur_str; delete cache_size_str; 
	}
};

class SqlApiBase
{
protected:
	// Static initialization performed
	static bool _static_init; 
	// Connection to the database is established
	bool _connected; 

	// Database subtype
	int _subtype;

	// Number of columns in open cursor
	size_t _cursor_cols_count;
	// Number of allocated rows for cursor
	size_t _cursor_allocated_rows;
	// Column describtions and data
	SqlCol *_cursor_cols;
	// A LOB column exists in the cursor
	bool _cursor_lob_exists;
	// Option to fetch LOB values as VARCHAR
	bool _cursor_fetch_lob_as_varchar;

	// Number of columns for bulk insert
	size_t _ins_cols_count;
	// Number of allocated rows for bulk insert
	size_t _ins_allocated_rows;
	// Column describtions and data
	SqlCol *_ins_cols;
	// Total number of already inserted rows
	int _ins_all_rows_inserted;

	// Catalog information
	std::list<SqlColMeta> _table_columns;
	std::list<SqlConstraints> _table_constraints;
	std::list<SqlConsColumns> _table_cons_columns;
	std::list<SqlComments> _table_comments;
	std::list<SqlIndexes> _table_indexes;
	std::list<SqlIndColumns> _table_ind_columns;
	std::list<SqlIndExp> _table_ind_expressions;
	std::list<SqlSequences> _sequences;
	std::list<std::string> _reserved_words_ddl;

	// Error information
	int _error;
	char _error_text[1024];

	// Native error information
	int _native_error;
	char _native_error_text[1024];
	
	// Source data provider (for LOB read/write in chunks), getting metadata
	SqlApiBase *_source_api_provider;
	short _source_api_type;
	short _source_api_subtype;

	// Target data provider (to get supported features to define how to bind data)
	SqlApiBase *_target_api_provider;

	// Full path of the loaded driver
	char _loaded_driver[1024];
	// Driver search paths
	std::list<std::string> _driver_paths;

	// Version string of the connected database
	std::string _version;

	// Version number (major.minor.release)
	int _version_major;
	int _version_minor;
	int _version_release;

	Parameters *_parameters;
	AppLog *_log;

	// The maximum number of rows in read/write batch, by default it is defined by the buffer size divided by the table row size
	size_t _batch_max_rows;

    // Fixed size buffer to read LOB values by binding not reading by parts (can cause truncation error if the buffer is less than
    // the maximum LOB value, default is to read LOBs by separate calls
    int _lob_bind_buffer;

	// When converting from ASCII or UTF16/UCS-2 character sets in the source database to UTF8 i.e. in the target database depending on 
	// the actual data you may need greater storage size. And vice versa converting in opposite direction you may require smaller storage size.
	// This parameter specifies the length change ratio. If the source length is 100, and ratio is 1.1 then the target length will be 110 
	float _char_length_ratio;

	// Library is in trace mode
	bool _trace;
	bool _trace_data;
	std::string _trace_data_file;

public:
	SqlApiBase();
	virtual ~SqlApiBase() {} 

	// Set the database subtype
	void SetSubType(int subtype) { _subtype = subtype; }

	// Initialize API for each thread
	virtual int Init() = 0;

    // Initialize session by setting general options
    void InitSession();

	// Set additional information about the driver type
	virtual void SetDriverType(const char *info) = 0;

	// Set the connection string in the API object
	virtual void SetConnectionString(const char *conn) = 0;

	// Connect to the database
	virtual int Connect(size_t *time_spent) = 0;

	// Disconnect from the database
	virtual void Disconnect() {}
	// Deallocate the driver
	virtual void Deallocate() {}

	// Get row count for the specified object
	virtual int GetRowCount(const char *object, int *count, size_t *time_spent) = 0;
	
	// Execute the statement and get scalar result
	virtual int ExecuteScalar(const char *query, int *result, size_t *time_spent) = 0;

	// Execute the statement
	virtual int ExecuteNonQuery(const char *query, size_t *time_spent) = 0;

	// Open cursor and allocate buffers
	virtual int OpenCursor(const char *query, size_t buffer_rows, int buffer_memory, size_t *col_count, size_t *allocated_array_rows, 
		int *rows_fetched, SqlCol **cols, size_t *time_spent, bool catalog_query = false,
        std::list<SqlDataTypeMap> *dtmap = NULL) = 0;

	// Fetch next portion of data to allocate buffers
	virtual int Fetch(int *rows_fetched, size_t *time_spent) = 0;

	// Close the cursor and deallocate buffers
	virtual int CloseCursor() = 0;

	// Initialize the bulk copy from one database into another
	virtual int InitBulkTransfer(const char *table, size_t col_count, size_t allocated_array_rows, SqlCol *s_cols, SqlCol **t_cols) = 0;

	// Transfer rows between databases
	virtual int TransferRows(SqlCol *s_cols, int rows_fetched, int *rows_written, size_t *bytes_written,
		size_t *time_spent) = 0;

	// Specifies whether API allows to parallel reading from this API and write to another API
	virtual bool CanParallelReadWrite() = 0;

	// Specifies whether target bound source buffers, so the same buffers must be passed to TransferRows
	virtual bool IsDataBufferBound() { return false; }

	// Get the maximum size of a character in the client character set in bytes (4 for UTF-8)
	virtual int GetCharMaxSizeInBytes() { return -1; }

	// Complete bulk transfer
	virtual int CloseBulkTransfer() = 0;

	// Drop the table
	virtual int DropTable(const char* table, size_t *time_spent, std::string &drop_stmt) = 0;
	// Remove foreign key constraints referencing to the parent table
	virtual int DropReferences(const char* table, size_t *time_spent) = 0;
	// Remove foreign key constraints on the specified child table
	virtual int DropReferencesChild(const char* /*table*/, size_t * /*time_spent*/, int * /*keys*/) { return -1; }

	// Get the length of LOB column in the open cursor
	virtual int GetLobLength(size_t row, size_t column, size_t *length) = 0;
	// Allocate the buffer to read the LOB value depending on character lengths
	virtual char* GetLobBuffer(size_t /*row*/, size_t /*column*/, size_t /*length*/, size_t * /*alloc_len*/) { return NULL; }
	virtual void FreeLobBuffer(char * /*buf*/) { return; }
	// Get LOB content
	virtual int GetLobContent(size_t row, size_t column, void *data, size_t length, int *len_ind) = 0;
	// Get partial LOB content
	virtual int GetLobPart(size_t row, size_t column, void *data, size_t length, int *len_ind) = 0;

	// Get the list of available tables
	virtual int GetAvailableTables(std::string &table_template, std::string &exclude, 
		std::list<std::string> &tables) = 0;

	// Read schema information
	virtual int ReadSchema(const char *select, const char *exclude, bool read_cns = true, bool read_idx = true) = 0;
	virtual int ReadSchemaForTransferTo(const char * /*select*/, const char * /*exclude*/) { return 0; }
	void ClearSchema();

	// Read specific catalog information
	virtual int ReadConstraintTable(const char *schema, const char *constraint, std::string &table) = 0;
	virtual int ReadConstraintColumns(const char *schema, const char *table, const char *constraint, std::string &cols) = 0;

	// Get a list of columns for specified primary or unique key
	virtual int GetKeyConstraintColumns(SqlConstraints & /*cns*/, std::list<std::string> & /*output*/, std::list<std::string> * = NULL) { return -1; }
	int GetKeyConstraintColumns(SqlConstraints &cns, std::string &output);

	// Get a list of columns for specified foreign key
	virtual int GetForeignKeyConstraintColumns(SqlConstraints & /*cns*/, std::list<std::string> & /*fcols*/, std::list<std::string> & /*pcols*/, std::string & /*ptable*/) { return -1; }
	
	// Get a list of columns for specified index
	int GetIndexColumns(SqlIndexes &idx, std::string &idx_cols); 
	virtual int GetIndexColumns(SqlIndexes & /*idx*/, std::list<std::string> & /*idx_cols*/, std::list<std::string> & /*idx_sorts*/) { return -1; }

    // Map the data type of column
    void MapDataType(SqlCol *col, std::list<SqlDataTypeMap> *dtmap);

	// Get connected database version
	std::string& GetVersion() { return _version; }
	int GetMajorVersion() { return _version_major; }
	int GetMinorVersion() { return _version_minor; }
	int GetReleaseVersion() { return _version_release; }

	bool IsVersionEqualOrHigher(int mj, int mi = 0, int rel = 0);

	// Get constraint by name
	virtual SqlConstraints* GetConstraint(const char *schema, const char *table, const char *name);

	// Split qualified name to schema and object name
	static void SplitQualifiedName(const char *qualified, std::string &schema, std::string &object);

	// Split connection string to user, password and database
	void SplitConnectionString(const char *conn, std::string &user, std::string &pwd, std::string &db);
	void SplitConnectionString(const char *conn, std::string &user, std::string &pwd, std::string &server, std::string &db, std::string &port);

	// Build a condition to select objects from the catalog
	static void GetSelectionCriteria(const char *select, const char *exclude, const char *schema, const char *object, std::string &output, const char *default_schema, bool upper_case);
	static void GetSelectionCriteria(const char *selection_template, const char *schema, const char *object, std::string &output, const char *default_schema, bool upper_case);

	// Get fatures supported by the provider
	virtual bool IsIntegerBulkBindSupported() { return true; }
	virtual bool IsSmallintBulkBindSupported() { return true; }
	virtual bool IsTimestampBulkBindSupported() { return true; }

	// Whether target database loads in UTF8
	virtual bool IsTargetUtf8LoadSupported() { return false; }

	// Return metadata
	std::list<SqlColMeta>* GetTableColumns() { return &_table_columns; }
	std::list<SqlConstraints>* GetTableConstraints() { return &_table_constraints; }
	std::list<SqlConsColumns>* GetConstraintColumns() { return &_table_cons_columns; }
	std::list<SqlComments>* GetTableComments() { return &_table_comments; }
	std::list<SqlIndexes>* GetTableIndexes() { return &_table_indexes; }
	std::list<SqlIndColumns>* GetIndexColumns() { return &_table_ind_columns; }
	std::list<SqlIndExp>* GetIndexExpressions() { return &_table_ind_expressions; }
	std::list<SqlSequences>* GetSequences() { return &_sequences; }

	// Check if identifier is a reserved word
	bool IsReservedWord(const char *name);
    bool IsReservedWord(std::string &name) { return IsReservedWord(name.c_str()); }
    
	// Get column default by table and column name
	virtual int GetColumnDefault(const char * /*table*/, const char * /*column*/, std::string & /*col_default*/) { return -1; }
	// Get table or column comment by table and column name
	virtual int GetComment(const char * /*table*/, const char * /*column*/, std::string & /*comment*/) { return -1; }

	// Set source and target database API provider
	void SetSourceApiProvider(SqlApiBase * /*api*/);
	void SetTargetApiProvider(SqlApiBase * /*api*/);

	// Get database type and subtype
	virtual short GetType() { return 0; }
	virtual short GetSubType() { return 0; }

	// Get the path of the loaded driver
	char *GetLoadedDriver() { return _loaded_driver; }
	// Get the driver search paths
	void GetDriverPaths(std::list<std::string> &paths); 

	void SetParameters(Parameters *p);
	void SetAppLog(AppLog *l) { _log = l; }

	// Get error information 
	int GetError() { return  _error; }
	char *GetNativeErrorText() { return _native_error_text; }
};

#endif // sqlines_sqlapibase_h