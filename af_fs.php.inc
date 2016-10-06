<?php


class af_fs {


	public static function mkdir($path, $mode=0777, $recursive=true, $context=NULL) {
		if (is_dir($path)) return true;
		$umask	= umask(0);
		$return	= mkdir($path, $mode, $recursive, $context);
		umask($umask);
		return $return;
	}


}
