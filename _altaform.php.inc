<?php

//TBX - THE RENDERING SYSTEM
require_once('_tbx/tbx.php.inc');

//ALTAFORM CONFIGURATION
require_once('af_config.php.inc');

//FILE SYSTEM RELATED FUNCTIONS
require_once('af_fs.php.inc');

//IP ADDRESS RELATED FUNCTIONS
require_once('af_ip.php.inc');

//AUDIT LOGGING
require_once('audit.php.inc');

//DEVICE DETECTION
require_once('device.php.inc');

//EXTRA STRING FUNCTIONS
require_once('string.php.inc');

//EXTRA TIME FUNCTIONS
require_once('time.php.inc');


//LIST OF ALTAFORM TRAITS - USED TO HELP ORGANIZE CODE
require_once('af_git.php.inc');
require_once('af_auth.php.inc');
require_once('af_robots.php.inc');
require_once('af_tbx.php.inc');
require_once('af_template.php.inc');




class		altaform
	extends	tbx {
	use		af_auth		{ postLogin as auth_postLogin; }
	use		af_robots;
	use		af_tbx;
	use		af_template;




	public function __construct($dbsession=false) {
		global $afconfig;

		parent::__construct();

		$afconfig->af				= $this;
		$this->config				= $afconfig;
		$this->_session				= $dbsession;
		$this->git					= new af_void;
		$this->device				= new afdevice;
		$this->_time				= time();
		$this->_path				= getcwd();
		$this->hhvm					= defined('HHVM_VERSION') ? 'HHVM' : 'PHP';
		$this->_headers['notice']	= [];

		if (substr($this->_path, -1) !== '/') $this->_path .= '/';

		//REPORT *ALL* ERRORS, WARNINGS, NOTICES ON DEVELOPMENT SERVERS
		if ($this->debug()) error_reporting(E_ALL);
	}




	public static function create() {
		return (new ReflectionClass(self::$class))
				->newInstanceArgs(func_get_args());
	}




	public static function cli() {
		global $get;
		if (php_sapi_name() === 'cli')					return true;
		if (defined('STDIN'))							return true;
		if (!$get instanceof getvar)					return false;
		if ($get->server('REQUEST_METHOD') !== NULL)	return false;
		return ((int)$get->server('argc')) > 0;
	}




	public static function ok($die=true) {
		echo 'AF-OK';
		if ($die) die();
	}




	public static function refresh($die=true) {
		echo 'AF-REFRESH';
		if ($die) die();
	}




	public function jq() {
		global $get;
		if (!isset($get)) return false;
		if (!is_a($get, 'getvar')) return false;
		return $get->bool('jq');
	}




	public function json($data) {
		$this->contentType('json');

		echo $data instanceof pudlResult
			? $data->json()
			: json_encode($data);
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




	public function contentType($extension) {
		global $db;

		if (headers_sent()) return $this;

		$list	= explode('.', $extension);
		$ext	= end($list);

		$type = $db->cellId(
			'pudl_mimetype',
			'mime_type',
			'mime_ext',
			$ext
		);

		header('Content-Type: ' . (
			$type!==false ? $type : ('application/'.$ext)
		));

		return $this;
	}




	public function postLogin() {
		global $user;

		$this->auth_postLogin();

		if ($this->debug()  ||  $user->isAdmin()) {
			$this->git = new af_git($this);
		}
	}




	//GET THE OBJECT TYPE, EITHER BY NUMBER OR NAME
	//PASS IN A NUMBER TO GET A NAME
	//PASS IN A NAME TO GET A NUMBER
	public static function type($name) {
		global $db;

		if (!is_array(self::$types)  ||  empty(self::$types)) {
			self::$types = $db->cache(AF_MINUTE*5)->collection('pudl_object_type');
		}

		if (!is_array(self::$types)) return false;

		if (is_int($name)  ||  ctype_digit($name)) {
			$name = (int) $name;
			return !empty(self::$types[$name]) ? self::$types[$name] : false;
		}

		return array_search($name, self::$types);
	}




	public function setting($key, $value=NULL) {
		global $db;

		//IF NO VALUE, RETURN EXISTING VALUE
		if (func_num_args() === 1) {
			return array_key_exists($key, $this->settings)
				? $this->settings[$key]
				: NULL;
		}

		//INSERT NEW VALUE
		$return = $db->upsert('pudl_altaform', [
			'af_key'	=> $key,
			'af_value'	=> $value,
		]);

		//PURGE VALUE CACHE FROM REDIS
		$db->purge('altaform_settings');

		return $return;
	}




	public function path() {
		return $this->_path;
	}




	public function time() {
		return $this->_time;
	}



	public function timeout($seconds) {
		global $db;
		set_time_limit($seconds);
		$db->timeout($seconds);
	}



	public function device() {
		return (string) $this->device;
	}



	public function debug() {
		return !empty($this->config->debug);
	}




	private				$_prefs		= [];
	protected			$_time		= 0;
	protected			$_path		= '';
	public				$git		= NULL;
	public				$config		= NULL;
	public				$device		= NULL;
	public				$settings	= [];
	public				$title		= '';
	public static		$types		= [];
	public static		$class		= 'altaform';
	public static		$version	= 'Altaform-Core 2.8.0';
}