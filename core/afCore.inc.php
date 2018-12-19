<?php


class		altaform
	extends	tbx {
	use		afAuth		{ postLogin as authLogin; }
	use		afRobots;
	use		afTemplate;
	use		afEncrypt;




	public function __construct($dbsession=false) {
		global $afconfig, $afurl;

		parent::__construct();

		// VERIFY REQUIRED EXTENSIONS ARE LOADED
		foreach (['ctype', 'json', 'session'] as $ext) {
			assertStatus(500,
				extension_loaded($ext),
				"The required PHP extension is missing: '$ext'"
			);
		}


		// INITIALIZE A BUNCH OF STUFF
		$afconfig->af				= $this;
		static::$af					= $this;
		$this->url					= $afurl;
		$this->config				= $afconfig;
		$this->_session				= $dbsession;
		$this->git					= new afVoid;
		$this->device				= new afDevice;
		$this->_time				= time();
		$this->_path				= getcwd();
		$this->hhvm					= defined('HHVM_VERSION') ? 'HHVM' : 'PHP';
		$this->_extension			= afCli() ? 'txt' : 'html';
		$this->_headers['notice']	= [];

		if (substr($this->_path, -1) !== '/') $this->_path .= '/';

		$this->_static				= $this->_path . 'static/';

		//REPORT *ALL* ERRORS, WARNINGS, NOTICES ON DEVELOPMENT SERVERS
		if ($this->debug()) error_reporting(E_ALL);
	}




	public static function create() {
		return (new ReflectionClass(self::$class))
				->newInstanceArgs(func_get_args());
	}




	public static function ok($die=true) {
		echo "AF-OK\n";
		if ($die) die();
	}




	public static function refresh($die=true) {
		echo "AF-REFRESH\n";
		if ($die) die();
	}




	public static function reload($die=true) {
		echo "AF-RELOAD\n";
		if ($die) die();
	}




	public static function afload($path, $die=true) {
		echo "AF-LOAD\n";
		echo static::$af->url($path, true);
		echo "\n";
		if ($die) die();
	}




	public static function redirect($path, $die=true) {
		echo "AF-REDIRECT\n";
		echo static::$af->url($path, true);
		echo "\n";
		if ($die) die();
	}




	public function jq() {
		global $get;
		if (!isset($get)) return false;
		if (!($get instanceof getvar)) return false;
		return $get->bool('jq');
	}




	public function json($data) {
		$this->contentType('json');

		echo ($data instanceof pudlData)
			? $data->json()
			: json_encode($data, JSON_PARTIAL_OUTPUT_ON_ERROR);
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




	public function contentType($extension=false) {
		if (empty($extension)) return $this->_extension;

		if (headers_sent()) return $this;

		$list	= explode('.', $extension);
		$ext	= new afMimetype(end($list));
		$item	= $ext->ext();

		$this->_extension = is_string($item) ? $item : end($list);

		header('Content-Type: ' . $ext . '; charset=utf-8');

		return $this;
	}




	public function postLogin() {
		global $user;

		$this->authLogin();

		if ($this->debug()  ||  $user->isAdmin()) {
			$this->git = new afGit($this);
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




	public function __get($name) {
		return $this->setting($name);
	}




	public function url($path, $base=false) {
		$url = $this->url;
		return $url($path, $base);
	}




	////////////////////////////////////////////////////////////////////////////
	// GET THE LOCAL PATH OF THE ALTAFORM-CORE LIBRARY
	////////////////////////////////////////////////////////////////////////////
	public static function dir() {
		return dirname(__DIR__);
	}




	public function path() {
		return $this->_path;
	}




	public function staticPath() {
		return $this->_static;
	}




	public function time() {
		return $this->_time;
	}



	public function timeout($seconds) {
		global $db;

		set_time_limit($seconds);

		if ($db instanceof pudl) {
			$db->timeout($seconds);
		}
	}



	public function device() {
		return (string) $this->device;
	}



	public function debug() {
		return !empty($this->config->debug);
	}




	private				$_extension	= 'html';
	protected			$_time		= 0;
	protected			$_path		= '';
	protected			$_static	= '';
	public				$git		= NULL;
	public				$url		= NULL;
	public				$config		= NULL;
	public				$device		= NULL;
	public				$settings	= [];
	public				$title		= '';
	public				$hhvm		= 'PHP';
	public static		$error		= false;
	public static		$af			= NULL;
	public static		$types		= [];
	public static		$class		= __CLASS__;
	public static		$version	= 'Altaform-Core 2.9.0';
}
