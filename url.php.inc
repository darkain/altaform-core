<?php


require_once('_closure/closure.php');



interface af_url { public function url(); }



class afurl {
	use closures;


	public function __construct() {
		global $get;

		$host = explode(':', $get->server('HTTP_HOST', ''));
		if (empty($host[0])) $host = explode(':', $get->server('SERVER_ADDR', ''));

		$this->directory	= getcwd();
		$this->uri			= urldecode($get->server('REQUEST_URI', ''));
		$this->domain		= reset($host);
		$this->referer		= $get->server('HTTP_REFERER', '');
		$this->https		= strtolower($get->server('HTTPS'));
		$this->https		= $this->https !== 'off'  &&  !empty($this->https);
		$this->protocol		= $this->https ? 'https' : 'http';
		$this->host			= $this->protocol . '://' . $this->domain;
		$this->af_host		= $this->host;
		$this->parts		= (array)parse_url($this->uri);
		$this->query		= str_replace(' ', '+', $this->uri . (empty($this->parts['query']) ? '?' : '&'));

		if (substr($this->uri, 0, 2) === '//') $this->redirect('/');

		if (empty($this->parts['path'])) error404();

		if ($this->parts['path'][0] !== '/') {
			$this->parts['path']  = '/' . $this->parts['path'];
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
				!in_array($val[0], ['.', '+', '-', '_', ' ', "\t", "\n", "\r", "\0", "\x0B", "\\"]),
				'Invalid character in URL path: 0x' . ord($val[0])
			);

			$this->url .= '/' . urlencode($val);
		}
	}



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



	public function __invoke($path, $base=false) {
		if ($path instanceof af_url  ||  !tbx_array($path)) {
			return ($base ? ($this->base.'/') : '') . $this->clean($path);
		}

		$url = $base ? ($this->base.'/') : '';

		if (empty($path)) return $url;

		foreach ($path as $item) $url .= $this->clean($item) . '/';

		return rtrim($url, '/');
	}



	public function build($path, $query=false, $host=false) {
		$return = ($host ? $this->host : '') . $this($path, true);
		if (empty($query))		return $return;
		if (is_string($query))	return $return . '?' . $query;
		return $return . '?' . http_build_query($query);
	}



	public function user($user, $base=false) {
		$user = empty($user['user_url']) ? $user['user_id'] : $user['user_url'];

		if (is_bool($base)) return $this($user, $base);

		$args = func_get_args();
		$args[0] = $user;

		return $this($args);
	}



	public function clean($fragment) {
		if ($fragment instanceof af_url) {
			$fragment = $fragment->url();
		}
		return ($fragment instanceof afurlsafe)
			? (string) $fragment
			: strtolower(urlencode($fragment));
	}



	public function vid($new_id=false) {
		if ($new_id !== false) $this->id = afstring::int($new_id);
		return $this->id;
	}



	public function reparse($prepend=[], $append=[], $replace=false) {
		if (empty($prepend)  &&  empty($append)) return;

		if (!tbx_array($prepend))	$prepend	= [$prepend];
		if (!tbx_array($append))	$append		= [$append];
		if ($replace)				$this->part	= [];

		$prepend	= array_reverse($prepend);

		$prepend[]	= '';
		$append[]	= '';

		array_shift($this->part);
		array_pop($this->part);

		foreach ($prepend as $item) {
			if ($item === false) continue;
			array_unshift($this->part, $item);
		}

		foreach ($append as $item) {
			if ($item === false) continue;
			$this->part[] = $item;
		}

		$this->reparse = true;
	}



	public function replace($prepend=[], $append=[]) {
		$this->reparse($prepend, $append, true);
	}



	public function finalize() {
		return ($this->reparse = NULL);
	}



	public function process() {
		global $af, $site;

		//Recursion limit
		static $recurse = 0;
		if ($recurse++ == 20) error500('INTERNAL REDIRECT RECURSION LIMIT REACHED');

		//Reset virtual pathing, in case this is a reprocess
		$this->virtual = [];

		//load our home page!!
		if (count($this->part) < 3) {
			return $this->reparse($this->homepage);
		}


		$count = count($this->part) - 1;

		if ($count === 2  &&  !empty($site['g_auth'][$this->part[1]])) {
			return print($site['g_auth'][$this->part[1]] . ' ' . $this->part[1]);
		}


		for ($i=1; $i<$count; $i++) {

			//FORCE VIRTUAL PATHING IF SPECIAL CHARACTERS ARE FOUND
			//SPECIAL CHARACTER ALLOWED: [SPACE] ! + - . _ (ALL OTHERS FORCE VITUAL PATHING)
			if (preg_match('/[^\x21\x2B\x2D\x2E\x5F 0-9a-zA-Z]/', $this->part[$i])) {
				if (is_dir('_virtual')) {
					$this->chdir( $this->virtualize($i, '_virtual') );
					if ($this->reparse  ||  $this->reparse === NULL) return true;
					if ($count-$i === 1) return $this->index();
				}

				if (is_file('_virtual.php'))	return $this->virtualize($i, '_virtual.php');
				if (is_file('_virtual.hh'))		return $this->virtualize($i, $this->hhvm('_virtual.hh'));
				error404();
			}


			if (is_dir($this->part[$i])) {
				$this->chdir( $this->part[$i] );
				if ($this->reparse  ||  $this->reparse === NULL) return true;
				if ($count-$i === 1) return $this->index();
				continue;
			}


			if ($count-$i === 1) {
				$file = $this->part[$i];
				if (is_file("$file.php"))		return "$file.php";
				if (is_file("$file.hh"))		return $this->hhvm("$file.hh");
				if (is_file("$file.tpl"))		return $af->renderPage("$file.tpl");
				if (is_file('_virtual.php'))	return $this->virtualize($i, '_virtual.php');
				if (is_file('_virtual.hh'))		return $this->virtualize($i, $this->hhvm('_virtual.hh'));
				if (!is_dir('_virtual'))		error404();

				$this->chdir( $this->virtualize($i, '_virtual') );
				if ($this->reparse  ||  $this->reparse === NULL) return true;

				return $this->index();
			}


			if (is_dir('_virtual')) {
				$this->chdir( $this->virtualize($i, '_virtual') );
				if ($this->reparse  ||  $this->reparse === NULL) return true;
				continue;
			}


			if (is_file('_virtual.php'))		return $this->virtualize($i, '_virtual.php');
			if (is_file('_virtual.hh'))			return $this->virtualize($i, $this->hhvm('_virtual.hh'));

			error404();
		}
	}



	private function virtualize($start, $return=false) {
		if (!empty($this->virtual)) return $return;

		$count = count($this->part)-1;
		if ($start >= $count) error500();

		for ($x=$start; $x<$count; $x++) {
			$this->virtual[] = $this->part[$x];
		}

		$this->vid($this->virtual[0]);

		return $return;
	}



	public static function cdn($hash, $key='hash') {
		global $afurl;
		$hash = self::cdnHash($hash, $key);
		if (empty($hash)) return false;
		$path = self::cdnPath($hash);
		return empty($path) ? false : $afurl->cdn . '/' . $path;
	}



	public static function cdnAll(&$list, $key='img', $hashkey='hash') {
		if (!tbx_array($list)) return false;
		if (empty($key)) $key = 'img';
		foreach ($list as &$val) {
			$val[$key] = self::cdn($val, $hashkey);
		} unset($val);
		return true;
	}



	public static function cdnPath($hash) {
		if (empty($hash)) return false;
		if (strlen($hash) === 16) $hash = bin2hex($hash);
		$hash = strtolower($hash);
		return	substr($hash, 0, 3) . '/' .
				substr($hash, 3, 3) . '/' .
				$hash;
	}



	public static function cdnFile($hash, $ext=false) {
		$hash = self::cdnHash($hash);
		if (empty($hash)) return false;
		if ($ext) return 'cdn/' . self::cdnPath($hash) . '.' . $ext;
		return 'cdn/' . self::cdnPath($hash);
	}



	public static function cdnHash($hash, $key='hash') {
		if (empty($hash))					return false;
		if (!tbx_array($hash))				return $hash;
		if (!empty($hash[$key]))			return $hash[$key];
		if (!empty($hash['thumb_hash']))	return $hash['thumb_hash'];
		if (!empty($hash['file_hash']))		return $hash['file_hash'];
		return false;
	}



	public static function post($url, $post=[], $options=[], $session=false) {
		$ch = curl_init();

		$agent = ini_get('user_agent');
		if (!empty($agent)  &&  is_string($agent)) {
			curl_setopt($ch, CURLOPT_USERAGENT, $agent);
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
			CURLOPT_POST			=> true,
			CURLOPT_POSTFIELDS		=> $post,
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



	//301 Moved Permanently
	//302 Found (temporary)
	//307 Temporary Redirect (keeps POST data)
	//308 Permanent Redirect (keeps POST data)
	public static function redirect($url, $type=301, $die=true) {
		global $afurl;
		if (tbx_array($url)) $url = $afurl($url, true);

		assert422(
			empty(array_intersect(
				str_split($url),
				["\r", "\n", "\t", "\0", '<', '>']
			))
		);

		if (!headers_sent()) header("Location: $url", true, $type);
		echo '<html><head><meta http-equiv="refresh" content="0;URL=\'';
		echo htmlspecialchars($url) . '\'" /></head><body>';
		echo "<script>top.location.href='";
		echo htmlspecialchars(addslashes($url)) . "';</script></body></html>";
		if ($die) exit;
	}



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



	//HANDLE SSL, TLS, HTTPS, HSTS RELATED CONFIG PROCESSING
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



	//SOURCE: http://stackoverflow.com/questions/1755144/how-to-validate-domain-name-in-php
	public static function validateDomain($domain) {
		if (!is_string($domain))	return false;
		if (strlen($domain) < 1)	return false;
		if (strlen($domain) > 253)	return false;
		if (!preg_match('/^([a-z\d](-*[a-z\d])*)(\.([a-z\d](-*[a-z\d])*))*$/i', $domain)) return false;
		return !!preg_match('/^[^\.]{1,63}(\.[^\.]{1,63})*$/', $domain);
	}



	//Move into a folder, and test security if needed
	private function chdir($__path) {
		assert500(@chdir($__path), 'Unable to enter directory');

		if (!is_file('_altaform.php.inc')) return;

		$__keys = array_keys($GLOBALS);
		extract($GLOBALS, EXTR_REFS | EXTR_SKIP);
		require('_altaform.php.inc');

		$list = get_defined_vars();

		$__keys[] = '__path';
		$__keys[] = '__keys';
		foreach ($__keys as $key) unset($list[$key]);

		foreach ($list as $key => $val) $GLOBALS[$key] = $val;
	}



	//Process index file, if available
	private function index() {
		global $af;
		if (is_file('_index.php'))	return '_index.php';
		if (is_file('_index.hh'))	return $this->hhvm('_index.hh');
		if (is_file('_index.tpl'))	return $af->renderPage('_index.tpl');
		error404();
	}



	private function hhvm($path) {
		if (defined('HHVM_VERSION')) return $path;
		error500('HHVM_VERSION is not defined. Cannot execute HACK language code from PHP.');
	}



	public function __debugInfo() {
		$dump = [];
		foreach ($this as $key => $item) {
			if (!is_object($item)) $dump[$key] = $item;
		}
		return $dump;
	}



	//SET IN CONSTRUCTOR
	public $directory;
	public $uri;
	public $domain;
	public $referer;
	public $https;
	public $protocol;
	public $host;
	public $af_host;
	public $part;
	public $parts;
	public $query;
	public $url;

	//SET DYNAMICALLY OR BY CONFIG
	public $path		= '';
	public $base		= '';
	public $cdn			= '';
	public $static		= '';
	public $upload		= '';
	public $push		= '';
	public $virtual		= [];
	public $id			= 0;
	public $jq			= false;
	public $reparse		= true;
	public $homepage	= 'homepage';
}


$afurl = new afurl;



class afurlsafe {
	public function __construct($string) { $this->string = $string; }
	public function __toString() { return $this->string; }
	private $string;
}
