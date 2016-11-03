<?php


class af_fs {


	public static function mkdir($path, $mode=0777, $recursive=true) {
		if (is_dir($path)) return true;
		$level	= error_reporting(E_ALL & ~E_WARNING);
		$umask	= umask(0);
		$return	= mkdir($path, $mode, $recursive);
		umask($umask);
		error_reporting($level);
		return $return;
	}


}
