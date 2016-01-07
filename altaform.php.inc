<?php

//Extra string functions
require_once('string.php.inc');

//Device detection
require_once('device.php.inc');

//Extra time functions
require_once('time.php.inc');

//TBX - the rendering system
require_once('_tbx/tbx_class.php');




class altaform extends clsTinyButXtreme {

	public function __construct() {
		parent::__construct();

		global $site;

		self::$_time		= time();
		self::$_path		= getcwd();
		if (substr(self::$_path, -1) !== '/') self::$_path .= '/';

		if (!empty($site['af_header'])) {
			$this->_header	= $site['af_header'];
		}
	}




	public static function create() {
		return new self::$class();
	}




	function cli() {
		global $get;
		if (php_sapi_name() === 'cli') return true;
		return ((int)$get->server('argc')) > 0;
	}




	function ok($die=true) {
		echo 'AF-OK';
		if ($die) die();
	}




	public static function redirect($url, $type=301, $die=true) {
		afurl::redirect($url, $type, $die);
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




	public function load($filename) {
		$device	= $filename . '.' . afdevice::device();
		if (file_exists($device)) return parent::load($device);

		$pathed	= $this->path() . $device;
		if (file_exists($pathed)) return parent::load($pathed);

		return parent::load($filename);
	}




	public function headerHTML() {
		global $site;

		$device	= afdevice::device();
		$root	= $this->path() . $site['root'];

		if ($this->debug()  &&  file_exists($root.'/header_html_debug.tpl.'.$device)) {
			$this->load($root.'/header_html_debug.tpl.'.$device);
		} else if ($this->debug()  &&  file_exists($root.'/header_html_debug.tpl')) {
			$this->load($root.'/header_html_debug.tpl');
		} else {
			$this->load($root.'/header_html.tpl');
		}

		$this->field('sheet',	implode(' ', $this->_sheet));
		$this->block('script',	$this->_script);
		$this->block('style',	$this->_style);
		$this->block('meta',	$this->_meta);
		return $this->render();
	}




	public function headerPage() {
		global $site;

		$device	= afdevice::device();
		$root	= $this->path() . $site['root'];

		if ($this->debug()  &&  file_exists($root.'/header_page_debug.tpl.'.$device)) {
			$this->load($root.'/header_page_debug.tpl.'.$device);
		} else if ($this->debug()  &&  file_exists($root.'/header_page_debug.tpl')) {
			$this->load($root.'/header_page_debug.tpl');
		} else {
			$this->load($root.'/header_page.tpl');
		}

		foreach ($this->_header as $key => $val) {
			$this->block($key, $val);
		}

		return $this->render();
	}




	public function header($key=false, $data=false, $replace=false) {
		if ($key === false) {
			return $this->headerHTML()->headerPage();
		}

		if ($replace  ||  empty($this->_header[$key])) {
			$this->_header[$key] = [$data];
		} else {
			$this->_header[$key][] = $data;
		}

		return $this;
	}




	public function headers($key, $data) {
		$this->_header[$key] = $data;
	}




	public function footerHTML() {
		global $site;

		$device	= afdevice::device();
		$root	= $this->path() . $site['root'];

		if ($this->debug()) {
			if (file_exists($root.'/footer_html_debug.tpl.'.$device)) {
				return $this->load($root.'/footer_html_debug.tpl.'.$device);
			}
			if (file_exists($root.'/footer_html_debug.tpl')) {
				return $this->render($root.'/footer_html_debug.tpl');
			}
		}

		return $this->render($root.'/footer_html.tpl');
	}




	public function footerPage() {
		global $site, $db;

		$device	= afdevice::device();
		$root	= $this->path() . $site['root'];

		if ($this->debug()  &&  file_exists($root.'/footer_page_debug.tpl.'.$device)) {
			$this->load($root.'/footer_page_debug.tpl.'.$device);
		} else if ($this->debug()  &&  file_exists($root.'/footer_page_debug.tpl')) {
			$this->render($root.'/footer_page_debug.tpl');
		} else {
			$this->render($root.'/footer_page.tpl');
		}

		if ($this->hasPermission('staff,admin')  &&  $this->debug()  &&  file_exists($root.'/footer_admin.tpl')) {
			$this->load($root.'/footer_admin.tpl');
			if (!empty($db)) {
				$this->field('dbstats',		$db->stats());
				$this->block('dbmisses',	$db->stats()['missed']);
			}
			$this->render();
		}

		return $this;
	}




	public function footer() {
		return $this->footerPage()->footerHTML();
	}




	public function renderPage($filename, $field=false, $data=false) {
		global $title, $og;

		//Allow device specific loading
		$device	= $filename . '.' . afdevice::device();

		//Pull the contents of the template BEFORE anything else!
		$text = file_get_contents( file_exists($device) ? $device : $filename );

		//PULL OUT REQUIRED PERMISSIONS TAG
		preg_match("/\<permission[^>]*>(.*)\<\/permission\>/", $text, $matches);
		if (!empty($matches[1])) {
			if ($matches[1] === 'login') {
				$this->requireLogin();
			} else {
				$this->requirePermission($matches[1]);
			}
			$text = preg_replace("/\<permission[^>]*>(.*)\<\/permission\>/", '', $text);
		}

		//PULL OUT TITLE TAG
		preg_match("/\<title[^>]*>(.*)\<\/title\>/", $text, $matches);
		if (!empty($matches[1])) {
			$title = $this->renderString($matches[1]);
			$text = preg_replace("/\<title[^>]*>(.*)\<\/title\>/", '', $text);
		}

		//PULL OUT DESCRIPTION TAG
		preg_match("/\<description[^>]*>(.*)\<\/description\>/", $text, $matches);
		if (!empty($matches[1])) {
			$og['description'] = $this->renderString($matches[1]);
			$text = preg_replace("/\<description[^>]*>(.*)\<\/description\>/", '', $text);
		}

		//PULL OUT IMAGE TAG
		preg_match("/\<image[^>]*>(.*)\<\/image\>/", $text, $matches);
		if (!empty($matches[1])) {
			$og['image'] = $this->renderString($matches[1]);
			$text = preg_replace("/\<image[^>]*>(.*)\<\/image\>/", '', $text);

			if (!empty($og['twittername'])  &&  !empty($og['twitterdomain'])) {
				$this->metas([
					['name'=>'twitter:card',		'content'=>'photo'],
					['name'=>'twitter:site',		'content'=>$og['twittername']],
					['name'=>'twitter:domain',		'content'=>$og['twitterdomain']],
					['name'=>'twitter:title',		'content'=>$title],
					['name'=>'twitter:image',		'content'=>$og['image']],
					['name'=>'twitter:description',	'content'=>$og['description']],
				]);
			}
		}

		//Render all the things!
		$this->header()->loadString($text);
			if ($field !== false  &&  $data !== false) {
				$this->field($field, $data);
			}
		$this->render()->footer();

		return $this;
	}




	//Add a JavaScript file to the html header
	public function script($path) {
		$this->_script[] = ['path' => $path];
		return $this;
	}




	//Add a Cascading Style Sheet file to the html header
	public function style($path) {
		$this->_style[] = ['path' => $path];
		return $this;
	}




	//Add inline Cascading Style Sheet html header
	public function sheet($styles) {
		$this->_sheet[] = $styles;
		return $this;
	}




	//Add extra custom meta tags to a page (such as Twitter Cards)
	public function meta($meta) {
		$this->_meta[] = $meta;
		return $this;
	}




	//Add extra custom meta tags to a page (such as Twitter Cards)
	public function metas($meta) {
		$this->_meta = array_merge($this->_meta, $meta);
		return $this;
	}




	public function updateUser($data) {
		global $db, $user;
		if (!$this->loggedIn()) return false;
		$return = $db->updateId('pudl_user', $data, 'user_id', $user);
		$this->purgeSession();
		return $return;
	}




	public function updateProfile($data) {
		global $db, $user;
		if (!$this->loggedIn()) return false;
		return $db->updateId('pudl_user_profile', $data, 'user_id', $user);
	}




	public function parseUserPreferences($string) {
		if (PHP_VERSION_ID >= 50400) {
			return json_decode($string, true, 512, JSON_BIGINT_AS_STRING);
		}

		return json_decode($string, true);
	}




	public function getUserPreferences($id=false) {
		global $user, $db;
		if (is_array($id)  &&  isset($id['user_id'])) $id = $id['user_id'];
		if (!is_numeric($id)) $id = $user['user_id'];
		$id = (int) $id;

		if (isset($this->_prefs[$id])) return $this->_prefs[$id];

		return $this->_prefs[$id] = self::parseUserPreferences(
			$db->cellId('pudl_user_preference', 'preference', 'user_id', $id)
		);
	}




	public function getUserPreference($path, $id=false, $default=false) {
		$prefs = $this->getUserPreferences($id);
		if (isset($prefs[ $path ])) return $prefs[ $path ];
		return $default;
	}




	public function setUserPreferences($preferences, $id=false) {
		global $user, $db;
		if (is_array($id)) $id = $id['user_id'];
		if (!is_numeric($id)) $id = $user['user_id'];
		$id = (int) $id;

		$this->_prefs[$id] = $preferences;

		return $db->replace('pudl_user_preference', [
			'user_id'		=> $id,
			'preference'	=> json_encode($preferences)
		], true);
	}




	public function updateUserPreference($key, $value, $id=false) {
		$prefs = self::getUserPreferences($id);
		$prefs[$key] = $value;
		return self::setUserPreferences($prefs, $id);
	}




	public function login($session=false) {
		global $user, $db, $get;

		$user = false;
		if (!isset($db)) return;

		if (empty($session)) $session = session_id();

		if (!empty($session)) {
			if ($id = (int) $get->session('AF:USER_ID')) {
				$user = $db->cache(60*60, "AF-SESSION-$session")->row(
					['us' => _pudl_user(200)],
					[
						'user_id'			=> $id,
						'user_permission'	=> pudl::inSet('user', 'staff', 'admin'),
					]
				);
			}
		}

		if (empty($user)) {
			$user = $db->cache(60*60)->rowId('pudl_user', 'user_id', 0);
			if (empty($user)) return;
		}

		$user['user_session'] = $session;
		if (empty($user['user_url'])) $user['user_url'] = $user['user_id'];

		$this->permissions($user);

		$this->postLogin();
	}




	public function postLogin() {}




	public function logout($session=false, $destroy=true) {
		global $user, $db;

		if (empty($session)) $session = session_id();

		if (!empty($session)) {
			$db->updateId('pudl_user', ['user_session'=>NULL], 'user_session', $session);
			$this->purgeSession();
		}

		$user = $db->rowId('pudl_user', 'user_id', 0);
		$this->permissions($user);

		if ($destroy) {
			if (ini_get('session.use_cookies')) {
				$params = session_get_cookie_params();
				setcookie(session_name(), '', $this->time() - 42000,
					$params['path'], $params['domain'],
					$params['secure'], $params['httponly']
				);
			}
			session_unset();
			session_destroy();
		}
	}




	public function purgeSession($session=false) {
		global $db;
		if (empty($session)) $session = session_id();
		$db->sync()->purge("AF-SESSION-$session");
	}




	public function loggedIn() {
		global $user;
		return !empty($user['user_id']);
	}




	public function requireLogin() {
		if (!$this->loggedIn()) error401();
		return $this;
	}




	public function hasPermission($permission, $account=false) {
		global $user, $db;

		if (!is_array($permission)) $permission = explode(',', $permission);
		if (empty($account)) $account = $user;

		if (is_int($account)  ||  is_string($account)) {
			$account = $db->rowId('pudl_user', 'user_id', $account);
			if (empty($account)) return false;
		}

		if (empty($account['permission'])) $this->permissions($account);

		foreach ($permission as $perm) {
			$perm = trim($perm);
			if (!empty($account['permission'][$perm])) return true;
		}

		return false;
	}




	public function requirePermission($permission, $account=false) {
		assert401(
			$this->hasPermission($permission, $account),
			'This page requires the following permission level: ' . $permission
		);
		return $this;
	}




	public function permissions(&$account) {
		global $site;

		//USER ACCESS RIGHTS
		$account['permission'] = $site['permission'];

		if (empty($account['user_permission'])) {
			$account['user_permission'] = 'guest';
		}

		$permissions = explode(',', $account['user_permission']);
		foreach ($permissions as $val) {
			$account['permission'][$val] = 1;
		}

		if (!empty($account['user_adfree'])) {
			if ($account['user_adfree'] > $this->time()) {
				$account['permission']['adfree'] = 1;
			}
		}
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




	//THIS IS DEPRICATED, REPLACED BY AFURL
	public function cdnUrl($hash, $key='file_hash') {
		return afurl::cdn($hash, $key);
	}




	//THIS IS DEPRICATED, REPLACED BY AFURL
	public function cdnUrlAll(&$list, $key='img', $hashkey='file_hash') {
		return afurl::cdnAll($list, $key, $hashkey);
	}




	protected function _customFormat(&$text, $style) {
		switch ($style) {
			case 'cdn':
				$text = $this->cdnUrl($text);
			break;

			case 'url':
				$text = afstring::url($text);
			break;

			case 'hex':
				$text = bin2hex($text);
			break;

			case 'urldate':
				$text = strtolower(date('Y/M', $text));
			break;

			default:
				parent::_customFormat($text, $style);
		}
	}




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




	//Log TinyButStrong error messages
	function meth_Misc_Alert($Src, $Msg, $NoErrMsg=false, $SrcType=false) {
		global $db, $get, $afurl;

		ob_start();
		$ret = parent::meth_Misc_Alert($Src, $Msg, $NoErrMsg, $SrcType);
		$text = trim(ob_get_clean());

		_af_log_error([
			'time'		=> date('r'),
			'file'		=> $this->filepath,
			'details'	=> $text,
			'url'		=> isset($afurl->all) ? $afurl->all : $get->server('REQUEST_URI'),
			'mariadb'	=> !empty($db) ? $db->server() : '',
			'proxy'		=> $get->server('SERVER_ADDR'),
			'server'	=> $get->server('X-PHP-IP'),
			'remote'	=> $get->server('REMOTE_ADDR'),
			'agent'		=> $get->server('HTTP_USER_AGENT'),
			'tbx'		=> json_encode($Src),
		], $this->debug());

		echo $text;
		return $ret;
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




	private				$_script	= [];
	private				$_style		= [];
	private				$_sheet		= [];
	private				$_meta		= [];
	private				$_prefs		= [];
	protected			$_header	= [];
	protected static	$_time		= 0;
	protected static	$_path		= '';
	public				$_footer	= 1;
	public static		$types		= [];
	public static		$class		= 'altaform';
	public static		$version	= 'Altaform 1.6.0';
}
