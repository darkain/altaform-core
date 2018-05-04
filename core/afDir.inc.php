<?php


class afDir {


	////////////////////////////////////////////////////////////////////////////
	// RECURSIVE MAKE DIRECTORY WITH WARNINGS IGNORED
	// this alleviates a race condition where directory is attempted to be made
	// with two different script instances at the same time
	////////////////////////////////////////////////////////////////////////////
	public static function create($path, $mode=0777, $recursive=true) {
		if (is_dir($path)) return true;
		$level	= error_reporting(E_ALL & ~E_WARNING);
		$umask	= umask(0);
		$return	= mkdir($path, $mode, $recursive);
		umask($umask);
		error_reporting($level);
		return $return;
	}




	////////////////////////////////////////////////////////////////////////////
	// CHECK IF PATH IS INDEED A DIR (NOT FILE) AND IS READABLE BY CURRENT USER
	////////////////////////////////////////////////////////////////////////////
	public static function readable($path) {
		return is_dir($path)  &&  is_readable($path);
	}




	////////////////////////////////////////////////////////////////////////////
	// CHECK IF PATH IS INDEED A DIR (NOT FILE) AND IS WRITABLE BY CURRENT USER
	////////////////////////////////////////////////////////////////////////////
	public static function writable($path) {
		return is_dir($path)  &&  is_writable($path);
	}


}
