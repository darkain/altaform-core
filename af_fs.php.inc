<?php


class af_fs {


	////////////////////////////////////////////////////////////////////////////
	// RECURSIVE MAKE DIRECTORY WITH WARNINGS IGNORED
	// this alleviates a race condition where directory is attempted to be made
	// with two different script instances at the same time
	////////////////////////////////////////////////////////////////////////////
	public static function mkdir($path, $mode=0777, $recursive=true) {
		if (is_dir($path)) return true;
		$level	= error_reporting(E_ALL & ~E_WARNING);
		$umask	= umask(0);
		$return	= mkdir($path, $mode, $recursive);
		umask($umask);
		error_reporting($level);
		return $return;
	}




	////////////////////////////////////////////////////////////////////////////
	// CHECK IF PATH IS INDEED A FILE (NOT DIR) AND IS READABLE BY CURRENT USER
	////////////////////////////////////////////////////////////////////////////
	public static function readfile($path) {
		return is_file($path)  &&  is_readable($path);
	}




	////////////////////////////////////////////////////////////////////////////
	// CHECK IF PATH IS INDEED A FILE (NOT DIR) AND IS WRITABLE BY CURRENT USER
	////////////////////////////////////////////////////////////////////////////
	public static function writefile($path) {
		return is_file($path)  &&  is_writable($path);
	}




	////////////////////////////////////////////////////////////////////////////
	// CHECK IF PATH IS INDEED A DIR (NOT FILE) AND IS READABLE BY CURRENT USER
	////////////////////////////////////////////////////////////////////////////
	public static function readdir($path) {
		return is_dir($path)  &&  is_readable($path);
	}




	////////////////////////////////////////////////////////////////////////////
	// CHECK IF PATH IS INDEED A DIR (NOT FILE) AND IS WRITABLE BY CURRENT USER
	////////////////////////////////////////////////////////////////////////////
	public static function writedir($path) {
		return is_dir($path)  &&  is_writable($path);
	}


}
