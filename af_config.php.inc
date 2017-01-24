<?php


require_once('af_object_array.php.inc');



class			af_config
	extends		af_object_array {




	public function __construct() {
		//Debugging is initially on, and disabled by init
		$this->debug		= $this;

		//Default to "online" mode
		$this->offline		= false;

		//Default site title
		$this->title		= 'Altaform';

		//Default application directory
		$this->root			= '_app';

		//Default no forced HTTPS (security)
		$this->secure		= false;

		//Default prevent framing site (security)
		$this->frames		= 'sameorigin';

		//Default allowed cross-site origins (security)
		$this->origins		= [
			'localhost', '127.0.0.1', '::1',
		];

		//Default timezone (localization)
		$this->timezone		= 'UTC';

		//Default locale (language / localization)
		$this->locale		= 'en_US.UTF-8';

		//Default session (cookies)
		$this->session		= [
			'name'			=> false,
			'domain'		=> false,
			'redirect'		=> 'root',
		];

		//Defaults for password complexity
		$this->password		= [
			'length'		=> 6,		//Minimum length
			'upper'			=> true,	//Require upper+lower case
			'number'		=> true,	//Require numbers
			'symbol'		=> false,	//Require special chararacters
		];

		//Default all permissions are disabled
		$this->permission	= [
			'banned'		=> 0,
			'guest'			=> 0,
			'pending'		=> 0,
			'user'			=> 0,
			'staff'			=> 0,
			'admin'			=> 0,
			'debug'			=> 0,
			'adfree'		=> 0,
		];
	}




	public function __invoke($data, $value=false) {
		if (empty($data)) return;

		if (func_num_args() === 1) {
			$this->_merge($data);

		} else if (is_array($this->{$data})) {
			$this->{$data} = array_merge($this->{$data}, $value);

		} else {
			$this->{$data} = $value;
		}
	}




	public function afkey($key=false) {
		static $afkey = NULL;
		if ($key === false) return $afkey;
		$oldkey	= $afkey;
		$afkey	= $key;
		return $oldkey;
	}



	public function auth($message) {
		return md5($this->af->time() . $message . $this->afkey());
	}



	public $af = NULL;

}



$afconfig = new af_config;
