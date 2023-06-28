<html>
<style type="text/css">
body { font-family: verdana; }
h1 { font-family: trebuchet ms, verdana; font-size: 20px; color: #000000; border-bottom:1px solid; }
h2 { font-family: trebuchet ms, verdana; font-size: 18px; color: #000000; border-bottom:1px solid; }
p { font-size: 13px; }
table { font-size: 13px; border: 1px solid; border-collapse:collapse; }
tr:hover td { background-color: #A4BBFE; }
th { background: #FFFFFF; padding: 0px 10px 0px 10px; border: 1px solid; }
td { padding: 0px 10px 0px 10px; border: 1px solid; }
.td_warn { background:yellow; }
a:link { text-decoration: none; color: #0000A0;}
a:visited { text-decoration: none; color: #0000A0;}
a:active { text-decoration: none; color: #0000A0;}
a:hover { text-decoration: none; color: #800517;}
</style>

<head>
<title>SQLines Report</title>
</head>

<body>
<h1>SQLines Report</h1>
<p><?summary?></p>

<!------------------------------->
<h2>Data Types</h2>

<p>All built-in data types:</p><?datatypes_table?>
<p>All derived and user-defined (UDT) data types:</p><?udt_datatypes_table?>
<p>Built-in data type conversion details:</p><?datatype_dtl_table?>
<p>Derived and user-defined (UDT) data type details:</p><?udt_datatype_dtl_table?>

<!------------------------------->
<h2>Functions</h2>

<p>All built-in functions:</p><?builtin_func_table?>
<p>All user-defined functions:</p><?udf_func_table?>
<p>Built-in function details:</p><?builtin_func_dtl_table?>

<!------------------------------->
<h2>Procedures</h2>

<p>All system procedures calls:</p><?system_proc_table?>
<p>System procedure call details:</p><?system_proc_dtl_table?>

<!------------------------------->
<h2>Statements</h2>

<p>All SQL statements:</p><?statements_table?>
<p>CREATE TABLE statements details:</p><?crtab_stmt_table?>
<p>ALTER TABLE statements details:</p><?alttab_stmt_table?>

<!------------------------------->
<h2>Procedural Language Statements</h2>

<p>All procedural SQL statements:</p><?pl_statements_table?>
</body>
</html>