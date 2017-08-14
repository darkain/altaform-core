<?php


require_once('af_closure.php.inc');
require_once('af_router.php.inc');



interface afi_url { public function url(); }



class		af_url {
	use		af_closure;
	use		af_router;




	////////////////////////////////////////////////////////////////////////////
	// PARSE THE URL AND QUERY STRING FRAGMENTS
	////////////////////////////////////////////////////////////////////////////
	public function __construct() {
		global $get;

		if (altaform::cli()) {
			ob_end_clean();
			$host = ['_cli'];
			$args = $get->server('argv');
			if (is_array($args)) {
				$_SERVER['REQUEST_URI'] = isset($args[1]) ? $args[1] : '/';
			}

		} else {
			$host = explode(':', $get->server('HTTP_HOST', ''));
			if (empty($host[0])) $host = explode(':', $get->server('SERVER_ADDR', ''));
			assert500($host[0] !== '_cli', 'RESTRICTED DOMAIN NAME');
		}

		$this->directory	= getcwd();
		$this->encoding		= array_map('trim', explode(',', $get->server('HTTP_ACCEPT_ENCODING','')));
		$this->uri			= urldecode($get->server('REQUEST_URI', ''));
		$this->domain		= strtolower(reset($host));
		$this->referer		= $get->server('HTTP_REFERER', '');
		$this->https		= strtolower($get->server('HTTPS', ''));
		$this->https		= $this->https !== 'off'  &&  !empty($this->https);
		$this->protocol		= $this->https ? 'https' : 'http';
		$this->host			= $this->protocol . '://' . $this->domain;
		$this->af_host		= $this->host;
		$this->parts		= (array)parse_url($this->uri);
		$this->query		= str_replace(' ', '+', $this->uri . (empty($this->parts['query']) ? '?' : '&'));

		if (in_array('gzip', $this->encoding)  &&  !afdevice::trident()) {
			$this->gz = '.gz';
		}

		if (substr($this->uri, 0, 2) === '//') $this->redirect('/');

		if (empty($this->parts['path'])) error404();

		if ($this->parts['path'][0] !== '/') {
			$this->parts['path'] = '/' . $this->parts['path'];
		}

		if ($this->parts['path'] === '/') {
			$this->url		= '/';
			$this->part		= [];
			return;		//EARLY OUT FOR HOMEPAGE, NO FOLDERS TO PROCESS!
		}

		if (substr($this->parts['path'], -1) === '/') {
			assert405(
				$get->server('REQUEST_METHOD') !== 'POST',
				'Attempting to redirect POST data. URL should not have trailing /'
			);
			$this->redirect(
				str_replace(' ', '+', substr($this->parts['path'], 0, -1)) .
				(empty($this->parts['query']) ? '' : ('?'.$this->parts['query']))
			);
		}

		$this->part		= explode('/', $this->parts['path']);
		$this->part[]	= '';
		foreach ($this->part as $val) {
			if (!strlen($val)) continue;

			assert500(
				!in_array($val[0], ['.', '+', '-', '_', "\\", 0x7F])
				&& ord($val[0])>0x20,
				'Invalid character in URL path: 0x' . ord($val[0])
			);

			$this->url .= '/' . urlencode($val);
		}
	}




	////////////////////////////////////////////////////////////////////////////
	// POST-CONFIGURATION (AF_CONFIG) OPTIONS
	////////////////////////////////////////////////////////////////////////////
	public function _all() {
		$base		= $this->host . $this->base;
		$this->full	= $base . $this->url;
		$this->all	= $this->full;
		if (!empty($this->parts['query'])  &&  $this->parts['query']!=='jq=1') {
			$this->all .= '?' . $this->parts['query'];
		}

		if (empty($this->cdn))		$this->cdn		= $base . '/cdn';
		if (empty($this->static))	$this->static	= $base . '/static';
		if (empty($this->upload))	$this->upload	= $base . '/upload';
	}




	////////////////////////////////////////////////////////////////////////////
	// GENERATE A URL
	////////////////////////////////////////////////////////////////////////////
	public function __invoke($path, $base=false) {
		if ($path instanceof afi_url  ||  !tbx_array($path)) {
			return ($base ? ($this->base.'/') : '') . $this->clean($path);
		}

		$url = $base ? ($this->base.'/') : '';

		if (empty($path)) return $url;

		foreach ($path as $item) $url .= $this->clean($item) . '/';

		return rtrim($url, '/');
	}




	////////////////////////////////////////////////////////////////////////////
	// BUILD A COMPLETE URL
	////////////////////////////////////////////////////////////////////////////
	public function build($path, $query=false, $host=false) {
		$return = ($host ? $this->host : '') . $this($path, true);
		if (empty($query))		return $return;
		if (is_string($query))	return $return . '?' . $query;
		return $return . '?' . http_build_query($query);
	}




	////////////////////////////////////////////////////////////////////////////
	// GET THE URL FOR THE GIVEN USER ACCOUNT
	////////////////////////////////////////////////////////////////////////////
	public function user($user, $base=false) {
		$user = empty($user['user_url']) ? $user['user_id'] : $user['user_url'];

		if (is_bool($base)) return $this($user, $base);

		$args = func_get_args();
		$args[0] = $user;

		return $this($args);
	}




	////////////////////////////////////////////////////////////////////////////
	// CLEAN PART OF A URL PATH FRAGMENT
	////////////////////////////////////////////////////////////////////////////
	public function clean($fragment) {
		if ($fragment instanceof afi_url) {
			$fragment = $fragment->url();
		}
		return ($fragment instanceof afurlsafe)
			? (string) $fragment
			: strtolower(urlencode($fragment));
	}




	////////////////////////////////////////////////////////////////////////////
	// GET THE CDN URL FOR THE GIVEN ITEM
	////////////////////////////////////////////////////////////////////////////
	public static function cdn($hash, $key='hash', $ext='mime') {
		global $afurl;

		if (!empty($hash[$ext])) {
			$ext = new af_mimetype($hash[$ext]);
		}

		$hash	= self::cdnHash($hash, $key);
		if (empty($hash)) return false;

		$path	= self::cdnPath($hash);
		if (empty($path)) return false;

		if (is_object($ext)  &&  $ext->id()) {
			$path .= '.' . $ext->ext();
		}

		return $afurl->cdn . '/' . $path;
	}




	////////////////////////////////////////////////////////////////////////////
	// PROCESS CDN URLS FOR AN ARRAY OF ITEMS
	////////////////////////////////////////////////////////////////////////////
	public static function cdnAll(&$list, $key='img', $hashkey='hash', $ext='mime') {
		if (!tbx_array($list)) return false;
		if (empty($key)) $key = 'img';
		foreach ($list as &$val) {
			$val[$key] = self::cdn($val, $hashkey, $ext);
		} unset($val);
		return true;
	}




	////////////////////////////////////////////////////////////////////////////
	// GET THE CDN PATH FOR THE GIVEN HASH
	////////////////////////////////////////////////////////////////////////////
	public static function cdnPath($hash) {
		if (empty($hash)) return false;
		if (strlen($hash) === 16) $hash = bin2hex($hash);
		$hash = strtolower($hash);
		return	substr($hash, 0, 3) . '/' .
				substr($hash, 3, 3) . '/' .
				$hash;
	}




	////////////////////////////////////////////////////////////////////////////
	// GET THE LOCAL CDN FILE PATH FOR THE GIVEN HASH
	////////////////////////////////////////////////////////////////////////////
	public static function cdnFile($hash, $ext=false) {
		$hash = self::cdnHash($hash);
		if (empty($hash)) return false;
		if ($ext) return 'cdn/' . self::cdnPath($hash) . '.' . $ext;
		return 'cdn/' . self::cdnPath($hash);
	}




	////////////////////////////////////////////////////////////////////////////
	// GET THE CDN HASH FROM THE GIVEN ARRAY OR VALUE
	////////////////////////////////////////////////////////////////////////////
	public static function cdnHash($hash, $key='hash') {
		if (empty($hash))					return false;
		if (!tbx_array($hash))				return $hash;
		if (!empty($hash[$key]))			return $hash[$key];
		if (!empty($hash['thumb_hash']))	return $hash['thumb_hash'];
		if (!empty($hash['file_hash']))		return $hash['file_hash'];
		return false;
	}




	////////////////////////////////////////////////////////////////////////////
	// INITIATE A POST REQUEST TO THE GIVEN URL
	////////////////////////////////////////////////////////////////////////////
	public static function post($url, $post=[], $options=[], $session=false) {
		$ch = curl_init();

		if (empty($options[CURLOPT_USERAGENT])) {
			$agent = ini_get('user_agent');
			if (empty($agent)) $agent = afdevice::agent();
			if (!empty($agent)) {
				curl_setopt($ch, CURLOPT_USERAGENT, $agent);
			}
		}

		if (!empty($session)) {
			curl_setopt($ch, CURLOPT_COOKIE, $session);
		}

		curl_setopt_array($ch, $options+[
			CURLOPT_URL				=> $url,
			CURLOPT_AUTOREFERER		=> true,
			CURLOPT_BINARYTRANSFER	=> true,
			CURLOPT_RETURNTRANSFER	=> true,
			CURLOPT_CONNECTTIMEOUT	=> 20,
			CURLOPT_TIMEOUT			=> 20,
			CURLOPT_POST			=> $post!==false,
			CURLOPT_POSTFIELDS		=> $post===false?false:$post,
			CURLOPT_HTTPHEADER		=> ['Expect:'],
		]);

		$contents			= curl_exec($ch);
		$data				= curl_getinfo($ch);
		$data['error']		= curl_error($ch);
		$data['errno']		= curl_errno($ch);
		$data['content']	= $contents;

		curl_close($ch);

		return $data;
	}




	////////////////////////////////////////////////////////////////////////////
	//REDIRECT CLIENT TO A NEW URL
	//301 Moved Permanently
	//302 Found (temporary)
	//307 Temporary Redirect (keeps POST data)
	//308 Permanent Redirect (keeps POST data)
	////////////////////////////////////////////////////////////////////////////
	public static function redirect($url, $type=301, $die=true) {
		global $af, $afurl;

		if (tbx_array($url)) $url = $afurl($url, true);

		assert422(
			empty(array_intersect(
				str_split($url),
				["\r", "\n", "\t", "\0", '<', '>']
			))
		);

		if (empty($af)  ||  !$af->jq()) {
			if (!headers_sent()) header("Location: $url", true, $type);
			echo '<html><head><meta http-equiv="refresh" content="0;URL=\'';
			echo tbx::html($url) . '\'" /></head><body>';
		}

		echo "<script>window.top.location.href=";
		echo json_encode($url);
		echo ";</script>";

		if (empty($af)  ||  !$af->jq()) {
			echo "</body></html>\n";
		}

		if ($die) exit;
	}




	////////////////////////////////////////////////////////////////////////////
	// REDIRECT CLIENT TO HTTPS SECURE VERSION OF PAGE
	////////////////////////////////////////////////////////////////////////////
	public function redirectSecure() {
		if (!headers_sent()) {
			header('Vary: upgrade-insecure-requests');
			header('Cache-Control: no-cache');
		}

		$this->redirect(
			'https' . substr($this->all, strlen($this->protocol)),
			307
		);
	}




	////////////////////////////////////////////////////////////////////////////
	//HANDLE SSL, TLS, HTTPS, HSTS RELATED CONFIG PROCESSING
	////////////////////////////////////////////////////////////////////////////
	public function secure($data) {
		if (headers_sent()) return;
		header('Content-Security-Policy: upgrade-insecure-requests');

		if (empty($data['max-age'])) return;

		$header = 'max-age=' . (int) $data['max-age'];

		if (in_array('includeSubDomains', $data)) {
			$header .= '; includeSubDomains';
		}

		if (in_array('preload', $data)) $header .= '; preload';

		header('Strict-Transport-Security: ' . $header);
	}




	////////////////////////////////////////////////////////////////////////////
	// VALIDATE THE GIVEN DOMAIN NAME
	//http://stackoverflow.com/questions/1755144/how-to-validate-domain-name-in-php
	////////////////////////////////////////////////////////////////////////////
	public static function validateDomain($domain) {
		if (!is_string($domain))	return false;
		if (strlen($domain) < 1)	return false;
		if (strlen($domain) > 253)	return false;
		if (!preg_match('/^([a-z\d](-*[a-z\d])*)(\.([a-z\d](-*[a-z\d])*))*$/i', $domain)) return false;
		return !!preg_match('/^[^\.]{1,63}(\.[^\.]{1,63})*$/', $domain);
	}




	////////////////////////////////////////////////////////////////////////////
	// LIMIT RECURSION FROM DEBUG INFO REQUESTS
	////////////////////////////////////////////////////////////////////////////
	public function __debugInfo() {
		$dump = [];
		foreach ($this as $key => $item) {
			if (!is_object($item)) $dump[$key] = $item;
		}
		return $dump;
	}




	////////////////////////////////////////////////////////////////////////////
	// MEMBER VARIABLES SET IN CONSTRUCTOR
	////////////////////////////////////////////////////////////////////////////
	public $directory;
	public $uri;
	public $domain;
	public $referer;
	public $https;
	public $protocol;
	public $host;
	public $af_host;
	public $parts;
	public $query;
	public $url;

	////////////////////////////////////////////////////////////////////////////
	// MEMBER VARIABLES SET DYNAMICALLY OR BY CONFIG
	////////////////////////////////////////////////////////////////////////////
	public $gz			= '';
	public $path		= '';
	public $base		= '';
	public $cdn			= '';
	public $static		= '';
	public $upload		= '';
	public $push		= '';
	public $encoding	= [];
	public $jq			= false;
}


$afurl = new af_url;



class afurlsafe {
	public function __construct($string) { $this->string = $string; }
	public function __toString() { return $this->string; }
	private $string;
}
