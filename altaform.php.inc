<?php

//Extra string functions
require_once('string.php.inc');

//Device detection
require_once('device.php.inc');

//Extra time functions
require_once('time.php.inc');

//Altaform Lite - the basic rendering system
require_once('altaform-lite.php.inc');

//Altaform Class Name - replace this if you inherit Altaform!
$altaform = 'altaform';


class altaform extends altaformlite {
	public function __construct() {
		parent::__construct();

		global $site;

		$this->SetOption('render', TBS_OUTPUT);
		$this->ResetVarRef(true);

		$this->_header	= empty($site['af_header']) ? [] : $site['af_header'];
		$this->_script	= [];
		$this->_style	= [];
		$this->_meta	= [];
		$this->_prefs	= [];
		$this->_footer	= 1;

		//Various data types Altaform manages
		require_once('types.php.inc');
	}


	function cli() {
		return (php_sapi_name() == 'cli' || (is_numeric($_SERVER['argc']) && $_SERVER['argc'] > 0));
	}


	function ok($wait=false, $die=true) {
		echo 'AF-OK';
		if ($wait) usleep(250000);
		if ($die) die();
	}



	//301=PERM, 302=TEMP
	public function redirect($url, $type=301, $die=true) {
		header("Location: $url", true, $type);
		echo("<script>top.location.href='$url';</script>");
		if ($die) exit;
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
		if (afdevice::mobile()  &&  file_exists($filename . '.mobile')) {
			$this->LoadTemplate($filename . '.mobile');
		} else if (afdevice::tablet()  &&  file_exists($filename . '.tablet')) {
			$this->LoadTemplate($filename . '.tablet');
		} else if (afdevice::tv()  &&  file_exists($filename . '.tv')) {
			$this->LoadTemplate($filename . '.tv');
		} else {
			$this->LoadTemplate($filename);
		}
	}




	public function headerHTML() {
		global $site;

		$root = $this->path() . $site['root'];
		if ($this->debug()  &&  file_exists("$root/header_html_debug.tpl")) {
			$this->load("$root/header_html_debug.tpl");
		} else {
			$this->load("$root/header_html.tpl");
		}

		$this->block('script',	$this->_script);
		$this->block('style',	$this->_style);
		$this->block('meta',	$this->_meta);
		$this->render();
	}


	public function headerPage() {
		global $site;

		$root = $this->path() . $site['root'];
		if ($this->debug()  &&  file_exists("$root/header_page_debug.tpl")) {
			$this->load("$root/header_page_debug.tpl");
		} else {
			$this->load("$root/header_page.tpl");
		}
		foreach ($this->_header as $key => &$val) {
			$this->block($key, $val);
		} unset($val);

		$this->render();
	}


	public function header($key=false, $data=false, $replace=false) {
		if ($key === false) {
			$this->headerHTML();
			$this->headerPage();
			return;
		}

		if ($replace  ||  empty($this->_header[$key])) {
			$this->_header[$key] = [$data];
		} else {
			$this->_header[$key][] = $data;
		}
	}


	public function footerHTML() {
		global $site;

		$root = $this->path() . $site['root'];
		if ($this->debug()  &&  file_exists("$root/footer_html_debug.tpl")) {
			$this->render("$root/footer_html_debug.tpl");
		} else {
			$this->render("$root/footer_html.tpl");
		}
	}


	public function footerPage() {
		global $site;

		$root = $this->path() . $site['root'];
		if ($this->debug()  &&  file_exists("$root/footer_page_debug.tpl")) {
			$this->render("$root/footer_page_debug.tpl");
		} else {
			$this->render("$root/footer_page.tpl");
		}
	}


	public function footer() {
		$this->footerPage();
		$this->footerHTML();
	}


	public function renderPage($filename, $field=false, $data=false) {
		global $title, $og;

		$text = file_get_contents($filename);

		//PULL OUT TITLE TAG
		preg_match("/\<title[^>]*>(.*)\<\/title\>/", $text, $matches);
		if (!empty($matches[1])) {
			$title = $matches[1];
			$text = preg_replace("/\<title[^>]*>(.*)\<\/title\>/", '', $text);
		}

		//PULL OUT DESCRIPTION TAG
		preg_match("/\<description[^>]*>(.*)\<\/description\>/", $text, $matches);
		if (!empty($matches[1])) {
			$og['description'] = $matches[1];
			$text = preg_replace("/\<description[^>]*>(.*)\<\/description\>/", '', $text);
		}

		//Render all the things!
		$this->header();

			//Load page's template BEFORE header
			$this->loadString($text);
			if ($field !== false  &&  $data !== false) {
				$this->field($field, $data);
			}
			$this->Show();

		$this->footer();
	}



	//Add a JavaScript file to the html header
	public function script($path) {
		$this->_script[] = ['path' => $path];
	}


	//Add a Cascading Style Sheet file to the html header
	public function style($path) {
		$this->_style[] = ['path' => $path];
	}


	//Add extra custom meta tags to a page (such as Twitter Cards)
	public function meta($meta) {
		$this->_meta[] = $meta;
	}


	//Add extra custom meta tags to a page (such as Twitter Cards)
	public function metas($meta) {
		$this->_meta = array_merge($this->_meta, $meta);
	}



	public function updateUser($data, $safe=false) {
		global $db, $user;
		if (!$this->loggedIn()) return false;
		return $db->update('pudl_user', $data, "`user_id`='$user[user_id]'", $safe);
	}


	public function updateProfile($data, $safe=false) {
		global $db, $user;
		if (!$this->loggedIn()) return false;
		return $db->update('pudl_user_profile', $data, "`user_id`='$user[user_id]'", $safe);
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
			'user_id' => $id,
			'preference' => json_encode($preferences)
		], true);
	}



	public function updateUserPreference($key, $value, $id=false) {
		$prefs = self::getUserPreferences($id);
		$prefs[$key] = $value;
		self::setUserPreferences($prefs, $id);
	}



	public function login($session=false) {
		global $user, $db, $site;

		if (!empty($site['af_login'])) {
			$site['af_login']($session);
			return;
		}

		if (!isset($db)) return;

		$session = $db->safe(empty($session) ? session_id() : $session);

		if (!empty($session)) {
			$user = $db->cache(60*60, "AF-SESSION-$session")->row(
				['us' => _pudl_user(50)],
				["user_session='$session'", ["user_permission='user'","user_permission='admin'"]]
			);
		}

		if (empty($user)) {
			$user = $db->cache(60*60)->rowId('pudl_user', 'user_id', 0);
			$user['messages'] = 0;
			$user['user_session'] = $session;

		} else {
			$user['img'] = $this->cdnUrl($user, 'thumb_hash');
			$user['user_class'] = 'logged-in';

			$user['messages'] = $db->count('pudl_notification', [
				"notification_user_to='$user[user_id]'",
				'notification_read=0'
			]);
		}

		if (empty($user['user_url'])) $user['user_url'] = $user['user_id'];

		$this->permissions($user);
	}


	public function logout($session=false, $destroy=true) {
		global $user, $db;

		$session = $db->safe(empty($session) ? session_id() : $session);

		if (!empty($session)) {
			$db->update('pudl_user', "user_session=NULL", "user_session='$session'");
			$db->purge("AF-SESSION-$session");
		}

		$user = $db->row('pudl_user', "user_id=0");
		$this->permissions($user);

		if ($destroy) {
			if (ini_get('session.use_cookies')) {
				$params = session_get_cookie_params();
				setcookie(session_name(), '', $this->time() - 42000,
					$params['path'], $params['domain'],
					$params['secure'], $params['httponly']
				);
			}

			session_destroy();
		}
	}


	public function loggedIn() {
		global $user;
		return !empty($user['user_id']);
	}


	public function requireLogin() {
		if (!$this->loggedIn()) error401();
	}


	public function hasPermission($permission, $theuser=false) {
		global $user;
		if (!is_array($theuser)) return (!empty($user['permission'][$permission]));
		if (empty($theuser['permission'])) $this->permissions($theuser);
		return (!empty($theuser['permission'][$permission]));
	}


	public function requirePermission($permission, $theuser=false) {
		if (!$this->hasPermission($permission, $theuser)) error401();
	}


	public function permissions(&$user) {
		global $site;

		//USER ACCESS RIGHTS
		$user['permission'] = $site['permission'];

		$types = explode(',', $user['user_permission']);
		foreach ($types as $key => $val) { $user['permission'][$val] = 1; }

		if (!empty($user['user_adfree'])) {
			if ($user['user_adfree'] > $this->time()) {
				$user['permission']['adfree'] = 1;
			}
		}
	}




	public function cdnUrl(&$data, $hashkey='file_hash') {
		global $afurl;

		if (is_array($data)) {
			$hash = $data[$hashkey];
		} else {
			$hash = $data;
		}

		if (empty($hash)) return false;

		if (strlen($hash) === 16) $hash = bin2hex($hash);
		$hash = strtolower($hash);

		$part1 = substr($hash, 0, 3);
		$part2 = substr($hash, 3, 3);

		return "$afurl->cdn/$part1/$part2/$hash";
	}


	public function cdnUrlAll(&$list, $key='img', $hashkey='file_hash') {
		if (!is_array($list)) return;
		if (empty($key)) $key = 'img';
		foreach ($list as $lk => &$val) {
			$val[$key] = $this->cdnUrl($val, $hashkey);
		}
	}


	protected function meth_Custom_Format(&$text, $style) {
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
				parent::meth_Custom_Format($text, $style);
		}
	}



	public function exif_read_data($source) {
		$exif = @exif_read_data($source);
		if (empty($exif)) return false;

		if (isset($exif['UndefinedTag:0xA430'])) {
			$exif['CameraOwnerName'] = $exif['UndefinedTag:0xA430'];
			unset($exif['UndefinedTag:0xA430']);
		}

		if (isset($exif['UndefinedTag:0xA431'])) {
			$exif['BodySerialNumber'] = $exif['UndefinedTag:0xA431'];
			unset($exif['UndefinedTag:0xA431']);
		}

		if (isset($exif['UndefinedTag:0xA432'])) {
			$exif['LensSpecification'] = $exif['UndefinedTag:0xA432'];
			unset($exif['UndefinedTag:0xA432']);
		}

		if (isset($exif['UndefinedTag:0xA433'])) {
			$exif['LensMake'] = $exif['UndefinedTag:0xA433'];
			unset($exif['UndefinedTag:0xA433']);
		}

		if (isset($exif['UndefinedTag:0xA434'])) {
			$exif['LensModel'] = $exif['UndefinedTag:0xA434'];
			unset($exif['UndefinedTag:0xA434']);
		}

		if (isset($exif['UndefinedTag:0xA435'])) {
			$exif['LensSerialNumber'] = $exif['UndefinedTag:0xA435'];
			unset($exif['UndefinedTag:0xA435']);
		}

		if (isset($exif['UndefinedTag:0x8830'])) {
			$exif['SensitivityType'] = $exif['UndefinedTag:0x8830'];
			unset($exif['UndefinedTag:0x8830']);
		}

		if (isset($exif['UndefinedTag:0x8832'])) {
			$exif['RecommendedExposureIndex'] = $exif['UndefinedTag:0x8832'];
			unset($exif['UndefinedTag:0x8832']);
		}

		return $exif;
	}


	public function getMeta($type, $id, $key=false) {
		global $db;

		if (!is_numeric($type)) {
			if (!isset($this->_types[$type])) return false;
			$type = $this->_types[$type];
		}
		$type = (int) $type;
		$id   = (int) $id;

		if (empty($key)) { //get ALL keys
			return $db->rows('pudl_meta', [
				"`meta_type`='$type'",
				"`meta_object`='$id'",
			]);
		}

		$key = $db->safe($key);
		return $db->rows('pudl_meta', [
			"`meta_type`='$type'",
			"`meta_object`='$id'",
			"`meta_key`='$key'",
		]);
	}


	public function setMeta($type, $id, $key, $value) {
		global $db;

		if (!is_numeric($type)) {
			if (!isset($this->_types[$type])) return false;
			$type = $this->_types[$type];
		}

		$db->insert('pudl_meta', [
			'meta_type'   => (int) $type,
			'meta_object' => (int) $id,
			'meta_key'    => $key,
			'meta_value'  => $value,
		], true, ['meta_value' => $value]);
	}



	function queue($service, $type, $data) {
		global $user, $db;

		if (empty($data)) {
			$db->delete('pudl_queue', [
				"`queue_user`='$user[user_id]'",
				"`queue_service`='$service'",
				"`queue_type`='$type'",
			]);
			return;
		}

		$db->insert(
			'pudl_queue',
			[
				'queue_user'	=> $user['user_id'],
				'queue_service'	=> $service,
				'queue_type'	=> $type,
				'queue_time'	=> $this->time(),
				'queue_message'	=> json_encode($data),
			],
			true,
			[
				'queue_time'	=> $this->time(),
				'queue_message'	=> json_encode($data),
				'queue_count'	=> ['INCREMENT'=>1],
			]
		);
	}




	//Log TinyButStrong error messages
	function meth_Misc_Alert($Src, $Msg, $NoErrMsg=false, $SrcType=false) {
		global $db, $get, $afurl;

		ob_start();
		$ret = parent::meth_Misc_Alert($Src, $Msg, $NoErrMsg, $SrcType);
		$text = ob_get_clean();

		_af_log_error([
			'time'		=> date('r'),
			'file'		=> $this->_LastFile,
			'details'	=> $text,
			'url'		=> isset($afurl->all) ? $afurl->all : $get->server('REQUEST_URI'),
			'mariadb'	=> !empty($db) ? $db->server() : '',
			'proxy'		=> $get->server('SERVER_ADDR'),
			'server'	=> $get->server('X-PHP-IP'),
			'remote'	=> $get->server('REMOTE_ADDR'),
			'agent'		=> $get->server('HTTP_USER_AGENT'),
		], false);

		echo $text;
		return $ret;
	}



	public function debug() {
		global $site;
		return (!empty($site['debug']));
	}


	private $_header;
	private $_script;
	private $_style;
	private $_meta;
	private $_prefs;
	private $_types;
	public  $_footer;
}
