<?php



class		afUrl {
	use		afCallable;




	////////////////////////////////////////////////////////////////////////////
	// PARSE THE URL AND QUERY STRING FRAGMENTS
	////////////////////////////////////////////////////////////////////////////
	public function __construct(\af\router $router) {
		global $get;

		if (function_exists('\af\cli')  &&  \af\cli()) {
			if (ob_get_level()) ob_end_clean();
			$host = ['_cli'];
			$args = $get->server('argv');
			if (is_array($args)) {
				$_SERVER['REQUEST_URI'] = isset($args[1]) ? $args[1] : '/';
			} else {
				$_SERVER['REQUEST_URI'] = '/';
			}

		} else {
			$host = explode(':', $get->server('HTTP_HOST', ''));
			if (empty($host[0])) $host = explode(':', $get->server('SERVER_ADDR', ''));
			$host[0] = strtolower($host[0]);
			assertStatus(400,
				$host[0] !== '_cli',
				'RESTRICTED DOMAIN NAME'
			);
		}

		$tmp				= strtolower($get->server('HTTPS', ''));
		$this->https		= !empty($tmp)  &&  $tmp !== 'off';

		$this->encoding		= array_map('trim', explode(',', $get->server('HTTP_ACCEPT_ENCODING','')));
		$this->uri			= $get->server('REQUEST_URI', '');
		$this->domain		= reset($host);
		$this->origin		= $get->server('HTTP_ORIGIN', '');
		$this->referer		= $get->server('HTTP_REFERER', '');
		$this->protocol		= $this->https ? 'https' : 'http';
		$this->host			= $this->protocol . '://' . $this->domain;
		$this->af_host		= $this->host;

		$router->parts		= (array)parse_url($this->uri);
		$this->query		= str_replace(' ', '+', $this->uri . (empty($router->parts['query']) ? '?' : '&'));

		if (in_array('gzip', $this->encoding)  &&  !afDevice::trident()) {
			$this->gz = '.gz';
		}

		if (substr($this->uri, 0, 2) === '//') $this->redirect('/');

		if (empty($router->parts['path'])) {
			if ($get instanceof getvar) {
				httpError(400, 'No path specified');
			} else {
				$router->parts['path'] = '/';
			}
		}

		if ($router->parts['path'][0] !== '/') {
			$router->parts['path'] = '/' . $router->parts['path'];
		}

		if ($router->parts['path'] === '/') {
			$this->url		= '/';
			$router->part		= [];
			return;		//EARLY OUT FOR HOMEPAGE, NO FOLDERS TO PROCESS!
		}

		if (substr($router->parts['path'], -1) === '/') {
			assertStatus(405,
				$get->server('REQUEST_METHOD') !== 'POST',
				'Attempting to redirect POST data. URL should not have trailing /'
			);
			$this->redirect(
				str_replace(' ', '+', substr($router->parts['path'], 0, -1)) .
				(empty($router->parts['query']) ? '' : ('?'.$router->parts['query']))
			);
		}

		$router->part		= explode('/', $router->parts['path']);
		$router->part[]	= '';
		foreach ($router->part as &$val) {
			$tmp = $val;

			$val = urldecode($val);

			if ($get instanceof getvar) {
				$val = $get->clean($val);
			}

			if (!strlen($val)) continue;

			$this->url .= '/' . $tmp;

			assertStatus(500,
				(	!in_array($val[0], ['.', '+', '-', '_', "\\", 0x7F])
					&& ord($val[0])>0x20	),
				'Invalid character in URL path: 0x' . dechex(ord($val[0]))
			);
		}
	}




	////////////////////////////////////////////////////////////////////////////
	// POST-CONFIGURATION (AFCONFIG) OPTIONS
	////////////////////////////////////////////////////////////////////////////
	public function _all(\af\router $router) {
		$base		= $this->host . $this->base;
		$this->full	= $base . $this->url;
		$this->all	= $this->full;
		if (!empty($router->parts['query'])  &&  $router->parts['query']!=='jq=1') {
			$this->all .= '?' . $router->parts['query'];
		}

		if (empty($this->cdn))		$this->cdn		= $base . '/cdn';
		if (empty($this->static))	$this->static	= $base . '/static';
		if (empty($this->upload))	$this->upload	= $base . '/upload';
	}




	////////////////////////////////////////////////////////////////////////////
	// GENERATE A URL
	////////////////////////////////////////////////////////////////////////////
	public function __invoke($path, $base=false) {
		$url	= '';
		$query	= '';

		if ($base === true) {
			$url = $this->base . '/';
		} else if ($base instanceof afUrl) {
			$url = $this->host . $this->base . '/';
		} else if (is_string($base)) {
			$url = $base . '/';
		}

		if ($path instanceof afUrlx  ||  !tbx_array($path)) {
			return $url . $this->clean($path);
		}


		//NO PATH, RETURN BASE URL
		if (empty($path)) return $url;


		//PARSE EACH URL PATH SEGMENT
		foreach ($path as $key => $value) {
			if (is_int($key)) {
				$url .= $this->clean($value) . '/';

			} else {
				$query .= ($query==='') ? '?' : '&';
				$query .= http_build_query(
					[$key => $value],
					NULL,
					ini_get('arg_separator.output'),
					PHP_QUERY_RFC3986
				);
			}
		}


		//RETURN URL
		return rtrim($url, '/') . $query;
	}




	////////////////////////////////////////////////////////////////////////////
	// BUILD A COMPLETE URL
	////////////////////////////////////////////////////////////////////////////
	public function build($path, $query=false, $host=false) {
		$return = ($host ? $this->host : '') . $this($path, true);
		if (empty($query))		return $return;
		if (is_string($query))	return $return . '?' . $query;

		return $return . '?' . http_build_query(
			$query,
			NULL,
			ini_get('arg_separator.output'),
			PHP_QUERY_RFC3986
		);
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
		if ($fragment instanceof afUrlx) {
			$fragment = $fragment->url();
		}
		return ($fragment instanceof afUrlSafe)
			? (string) $fragment
			: strtolower(urlencode($fragment));
	}




	////////////////////////////////////////////////////////////////////////////
	// GET THE CDN URL FOR THE GIVEN ITEM
	////////////////////////////////////////////////////////////////////////////
	public static function cdn($hash, $key='hash', $ext='mime') {
		global $afurl, $db;

		if (!empty($hash[$ext])) {
			$ext = new \af\mime($db, $hash[$ext]);
		}

		$hash	= static::cdnHash($hash, $key);
		if (empty($hash)) return false;

		$path	= static::cdnPath($hash);
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
			$val[$key] = static::cdn($val, $hashkey, $ext);
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
		$hash = static::cdnHash($hash);
		if (empty($hash)) return false;
		if ($ext) return 'cdn/' . static::cdnPath($hash) . '.' . $ext;
		return 'cdn/' . static::cdnPath($hash);
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
	// INITIATE A GET REQUEST TO THE GIVEN URL
	////////////////////////////////////////////////////////////////////////////
	public static function get($url, $options=[], $maxlen=NULL) {
		$ctx = stream_context_create(['http' => $options]);
		return !empty($maxlen)
			? @file_get_contents($url, false, $ctx, 0, $maxlen)
			: @file_get_contents($url, false, $ctx);
	}




	////////////////////////////////////////////////////////////////////////////
	// INITIATE A POST REQUEST TO THE GIVEN URL
	////////////////////////////////////////////////////////////////////////////
	public static function post($url, $post=[], $options=[], $session=false) {
		$ch = curl_init();

		if (empty($options[CURLOPT_USERAGENT])) {
			$agent = ini_get('user_agent');
			if (empty($agent)) $agent = afDevice::agent();
			curl_setopt(
				$ch,
				CURLOPT_USERAGENT,
				!empty($agent) ? $agent : ('Altaform ' . altaform::$version)
			);
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
	// REDIRECT CLIENT TO A NEW URL
	// 301 Moved Permanently
	// 302 Found (temporary)
	// 307 Temporary Redirect (keeps POST data)
	// 308 Permanent Redirect (keeps POST data)
	////////////////////////////////////////////////////////////////////////////
	public static function redirect($url, $code=301, $die=true) {
		global $af, $afurl;

		if (tbx_array($url)) $url = $afurl($url, true);

		$intersect = array_intersect(
			str_split((string)$url),
			["\r", "\n", "\t", "\0", '<', '>']
		);
		assertStatus(422, empty($intersect));


		if (function_exists('\af\cli')  &&  \af\cli()) {
			echo 'Location: ' . $url;

		} else {
			if (empty($af)  ||  !$af->jq()) {
				if (!headers_sent()) header('Location: '.$url, true, $code);
				echo '<html><head><meta http-equiv="refresh" content="0;URL=\'';
				echo tbx::html($url) . '\'" /></head><body>';
			}

			echo "<script>window.top.location.href=";
			echo json_encode($url);
			echo ";</script>";

			if (empty($af)  ||  !$af->jq()) {
				echo "</body></html>\n";
			}
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
	public $uri;
	public $domain;
	public $origin;
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
	public $all			= '';
	public $full		= '';
	public $base		= '';
	public $cdn			= '';
	public $static		= '';
	public $upload		= '';
	public $push		= '';
	public $encoding	= [];
	public $jq			= false;
}



////////////////////////////////////////////////////////////////////////////////
// CREATE GLOBAL INSTANCE OF AFURL
////////////////////////////////////////////////////////////////////////////////
$afurl = new afUrl($router);




////////////////////////////////////////////////////////////////////////////////
// ??
////////////////////////////////////////////////////////////////////////////////
interface afUrlx {
	public function url();
}




////////////////////////////////////////////////////////////////////////////////
// ??
////////////////////////////////////////////////////////////////////////////////
class afUrlSafe {
	public function __construct($string) { $this->string = $string; }
	public function __toString() { return $this->string; }
	private $string;
}
