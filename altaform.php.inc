<?php

//AUDIT LOGGING
require_once('audit.php.inc');

//DEVICE DETECTION
require_once('device.php.inc');

//EXTRA STRING FUNCTIONS
require_once('string.php.inc');

//EXTRA TIME FUNCTIONS
require_once('time.php.inc');

//TBX - THE RENDERING SYSTEM
require_once('_tbx/tbx.php.inc');


//LIST OF ALTAFORM TRAITS - USED TO HELP ORGANIZE CODE
require_once('af_auth.php.inc');
require_once('af_robots.php.inc');
require_once('af_tbx.php.inc');
require_once('af_template.php.inc');



class altaform extends tbx {
	use af_auth;
	use af_robots;
	use af_tbx;
	use af_template;




	public function __construct($dbsession=false) {
		parent::__construct();

		$this->_session		= $dbsession;
		self::$_time		= time();
		self::$_path		= getcwd();
		if (substr(self::$_path, -1) !== '/') self::$_path .= '/';
	}




	public static function create() {
		return (new ReflectionClass(self::$class))->newInstanceArgs(func_get_args());
	}




	public static function cli() {
		global $get;
		if (php_sapi_name() === 'cli') return true;
		return ((int)$get->server('argc')) > 0;
	}




	public function ok($die=true) {
		echo 'AF-OK';
		if ($die) die();
	}




	//TODO: THIS IS DEPRECATED, AND SHOULD BE HANDLED BY $AFURL
	public function jq() {
		global $get;
		if (!isset($get)) return false;
		if (!is_a($get, 'getvar')) return false;
		return $get->bool('jq');
	}




	public function json($data) {
		if (!headers_sent()) header('Content-Type: application/json');
		echo ($data instanceof pudlResult) ? $data->json() : json_encode($data);
	}




	public function tempnam($path, $prefix='', $suffix='') {
		global $user;
		if (substr($path, -1) !== '/'  &&  substr($path, -1) !== '\\') {
			$path .= '/';
		}

		return $path . implode('_', [
			$prefix,
			$user['user_id'],
			rand(),
			microtime(true)
		]) . $suffix;
	}




	//GET THE OBJECT TYPE, EITHER BY NUMBER OR NAME
	//PASS IN A NUMBER TO GET A NAME
	//PASS IN A NAME TO GET A NUMBER
	public function type($name) {
		if (is_int($name)  ||  ctype_digit($name)) {
			$name = (int) $name;
			return !empty(self::$types[$name]) ? self::$types[$name] : false;
		}
		return array_search($name, self::$types);
	}




	//TODO: PLACE THIS IS DEDICATED AFUSER CLASS
	function queue($service, $type, $data) {
		global $user, $db;

		if (empty($data)) {
			$db->delete('pudl_queue', [
				'queue_user'	=> $user['user_id'],
				'queue_service'	=> $service,
				'queue_type'	=> $type,
			]);
			return;
		}

		//TODO: CONVERT THIS OVER TO NEW DYANMIC COLUMN FORMAT
		$db->insert('pudl_queue', [
				'queue_user'	=> $user['user_id'],
				'queue_service'	=> $service,
				'queue_type'	=> $type,
				'queue_time'	=> $this->time(),
				'queue_message'	=> json_encode($data),
			], [
				'queue_time'	=> $this->time(),
				'queue_message'	=> json_encode($data),
				'queue_count'	=> pudl::_increment(1),
			]
		);
	}




	public function path() {
		return self::$_path;
	}




	public function time() {
		return self::$_time;
	}




	public function debug() {
		global $site;
		return (!empty($site['debug']));
	}




	private				$_prefs		= [];
	protected static	$_time		= 0;
	protected static	$_path		= '';
	public static		$types		= [];
	public static		$class		= 'altaform';
	public static		$version	= 'Altaform 2.4.3';
}
