<?php



if (!class_exists('pudlObject')) {
	require_once(is_owner('_pudl/pudlObject.php'));
	require_once(is_owner('_tbx/tbx_plugin.inc.php'));
}




class			afConfig
	extends		pudlObject
	implements	tbx_plugin {




	public function __construct() {
		//Debugging is initially on, and disabled by init
		$this->debug		= $this;

		//Default to "online" mode
		$this->offline		= false;

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
			$this->merge($data);

		} else if (is_array($this->{$data})) {
			$this->{$data} = array_merge($this->{$data}, $value);

		} else {
			$this->{$data} = $value;
		}
	}




	public function afkey($key=false) {
		static $afkey = NULL;
		if ($key === false) return (string)$afkey;
		$oldkey	= $afkey;
		$afkey	= $key;
		return (string)$oldkey;
	}




	public function auth($message, $time=false, $algorithm='sha1') {
		if ($time === true)		$time = $this->af->time();
		if (!is_bool($time))	$time = (int) ($time / AF_MINUTE);
		return hash_hmac($algorithm, $time.$message, $this->afkey());
	}




	public function verify($hash, $message, $time=false, $algorithm='sha1') {
		$afkey = $this->afkey();

		if ($time === true) $time = $this->af->time();

		if (is_bool($time)) {
			return hash_equals(
				hash_hmac($algorithm, $message, $afkey),
				$hash
			);
		}

		$time = (int) ($time / AF_MINUTE);
		for ($x=$time-5; $x<=$time+5; $x++) {
			if (hash_equals(
				hash_hmac($algorithm, $x.$message, $afkey),
				$hash
			)) return true;
		}

		return false;
	}




	public function tbx_render($array) {
		return $this->af->setting('af.' . implode('.', $array));
	}



	public function onload($callback) {
		$this->callbacks[] = $callback;
	}


	public function _onload() {
		foreach ($this->callbacks as $callback) {
			$callback($this);
		}
	}



	public	$af			= NULL;
	private	$callbacks	= [];

}



$afconfig = new afConfig;
