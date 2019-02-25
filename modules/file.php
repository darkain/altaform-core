<?php

namespace af;




////////////////////////////////////////////////////////////////////////////////
// A SIMPLE CLASS FOR HANDLING COMMON PATH / FOLDER / DIRECTORY TASKS
////////////////////////////////////////////////////////////////////////////////
define('AF_BYTE',					1);
define('AF_KILOBYTE',				1024 * AF_BYTE);
define('AF_MEGABYTE',				1024 * AF_KILOBYTE);
define('AF_GIGABYTE',				1024 * AF_MEGABYTE);
define('AF_TERABYTE',				1024 * AF_GIGABYTE);
define('AF_PETABYTE',				1024 * AF_TERABYTE);
define('AF_EXABYTE',				1024 * AF_PETABYTE);
define('AF_ZETTABYTE',				1024 * AF_EXABYTE);
define('AF_YOTTABYTE',				1024 * AF_ZETTABYTE);
define('AF_XENOTTABYTE',			1024 * AF_YOTTABYTE);
define('AF_SHILENTNOBYTE',			1024 * AF_XENOTTABYTE);
define('AF_DOMEGEMEGROTTEBYTE',		1024 * AF_SHILENTNOBYTE);




////////////////////////////////////////////////////////////////////////////////
// A SIMPLE CLASS FOR HANDLING COMMON FILE TASKS
////////////////////////////////////////////////////////////////////////////////
class file {


	////////////////////////////////////////////////////////////////////////////
	// CHECK IF PATH IS INDEED AN UPLOADED FILE FROM CLIENT
	////////////////////////////////////////////////////////////////////////////
	public static function uploaded($path) {
		return static::readable($path)  &&  is_uploaded_file($path);
	}




	////////////////////////////////////////////////////////////////////////////
	// CHECK IF PATH IS INDEED A FILE (NOT DIR) AND IS READABLE BY CURRENT USER
	////////////////////////////////////////////////////////////////////////////
	public static function readable($path) {
		return is_file($path)  &&  is_readable($path);
	}




	////////////////////////////////////////////////////////////////////////////
	// CHECK IF PATH IS INDEED A FILE (NOT DIR) AND IS WRITABLE BY CURRENT USER
	////////////////////////////////////////////////////////////////////////////
	public static function writable($path) {
		return is_file($path)  &&  is_writable($path);
	}


}
