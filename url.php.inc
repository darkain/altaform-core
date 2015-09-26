<?php

require_once('_closure/closure.php');


class afurl {
	use closures;


	public function construct($db=false) {
		global $get, $site;

		$this->directory = getcwd();
		$this->db = $db;

		//Configurable paths
		if (!empty($site['base']))		$this->base		= $site['base'];
		if (!empty($site['cdn']))		$this->cdn		= $site['cdn'];
		if (!empty($site['static']))	$this->static	= $site['static'];
		if (!empty($site['upload']))	$this->upload	= $site['upload'];
		if (!empty($site['img']))		$this->img		= $site['img'];

		$this->uri		= urldecode($get->server('REQUEST_URI', '', _GETVAR_BASIC));
		$this->domain	= $get->server('HTTP_HOST', '', _GETVAR_BASIC);
		$this->referer	= $get->server('HTTP_REFERER', '', _GETVAR_BASIC);
		$this->https	= strtolower($get->server('HTTPS'));
		$this->https	= ($this->https !== 'off'  &&  !empty($this->https));
		$this->host		= ($this->https?'https://':'http://') . $this->domain;
		$this->af_host	= $this->host;

		$this->parts	= (array)parse_url($this->uri);
		if (empty($this->parts['path'])) error404();

		$this->query	= $this->uri . (empty($this->parts['query']) ? '?' : '&');
		$this->query	= str_replace(' ', '+', $this->query);

		if (substr($this->parts['path'],  0, 1) !== '/') $this->parts['path']  = '/' . $this->parts['path'];
		if (substr($this->parts['path'], -1, 1) !== '/') $this->parts['path'] .= '/';
		if (substr($this->parts['path'],  1, 1) === '_') error500();

		$this->part = explode('/', $this->parts['path']);
		$this->url = '/';
		foreach ($this->part as $key => $val) {
			if (strlen($val) < 1) continue;
			$this->url .= urlencode($val) . '/';
		}

		$this->all	= $this->host . $this->base . $this->url;
		$this->full	= $this->all;
		if (!empty($this->parts['query'])  &&  $this->parts['query']!=='jq=1') {
			$this->all .= '?' . $this->parts['query'];
		}
	}


	public function __invoke($path, $base=false) {
		if (!is_array($path)) {
			return ($base ? ($this->base.'/') : '') . $this->clean($path)  . '/';
		}

		$url = $base ? ($this->base.'/') : '';
		foreach ($path as $item) $url .= $this->clean($item)  . '/';
		return $url;
	}



	public function clean($fragment) {
		return strtolower(urlencode($fragment));
	}



	public function vid($new_id=false) {
		if ($new_id !== false) $this->id = afstring::int($new_id);
		return $this->id;
	}


	public function user($user) {
		return $this(
			empty($user['user_url']) ? $user['user_id'] : $user['user_url']
		);
	}



	public function reparse($prepend=[], $append=[]) {
		if (empty($prepend)  &&  empty($append)) return;

		if (!is_array($prepend)) $prepend = [$prepend];
		if (!is_array($append))  $append  = [$append];

		$prepend[] = '';
		$append[]  = '';

		array_shift($this->part);
		array_pop($this->part);

		foreach ($prepend as $item) {
			array_unshift($this->part, $item);
		}

		foreach ($append as $item) {
			$this->part[] = $item;
		}

		$this->reparse = true;
	}



	public function process() {
		global $site, $af;

		//Reset virtual pathing, in case this is a reprocess
		$this->virtual = [];


		//	load our web page!!
		if (count($this->part) < 3) {
			if (is_file("$site[root]/index.php")) {
				return "$site[root]/index.php";

			} else if (is_file("$site[root]/index.hh")) {
				return "$site[root]/index.hh";

			} else if (is_file("$site[root]/index.tpl")) {
				$af->renderPage("$site[root]/index.tpl");
				return true;

			} else {
				error404();
			}
		}


		$count = count($this->part) - 1;

		for ($i=1; $i<$count; $i++) {
			if (is_dir($this->part[$i])) {

				chdir($this->part[$i]);
				if ($count-$i == 1) {
					if (is_file('index.php')) {
						return 'index.php';

					} else if (is_file('index.hh')) {
						return 'index.hh';

					} else if (is_file('index.tpl')) {
						$af->renderPage('index.tpl');
						return true;

					} else {
						error404();
					}
				}

			} else if ($count-$i == 1) {

				$file = $this->part[$i];
				if (is_file("$file.php")) {
					return "$file.php";

				} else if (is_file("$file.hh")) {
					return "$file.hh";

				} else if (is_file("$file.tpl")) {
					$af->renderPage("$file.tpl");
					return true;

				} else if (is_file('virtual.php')) {
					$this->virtualize($i);
					return 'virtual.php';

				} else if (is_file('virtual.hh')) {
					$this->virtualize($i);
					return 'virtual.hh';

				} else if (is_dir('virtual')) {
					$this->virtualize($i);
					chdir('virtual');

					if ($count-$i == 1) {
						if (is_file('index.php')) {
							return 'index.php';

						} else if (is_file('index.hh')) {
							return 'index.hh';

						} else if (is_file('index.tpl')) {
							$af->renderPage('index.tpl');
							return true;

						} else {
							error404();
						}
					}

				} else {
					error404();
				}

			} else if (is_dir('virtual')) {
				$this->virtualize($i);
				chdir('virtual');

			} else if (is_file('virtual.php')) {
				$this->virtualize($i);
				return 'virtual.php';

			} else if (is_file('virtual.hh')) {
				$this->virtualize($i);
				return 'virtual.hh';

			} else {
				error404();
			}
		}

	}



	private function virtualize($start) {
		if (!empty($this->virtual)) return;

		$count = count($this->part)-1;
		if ($start >= $count) error500();

		for ($x=$start; $x<$count; $x++) {
			$this->virtual[] = $this->part[$x];
		}

		$this->vid($this->virtual[0]);
	}



	public static function cdn($hash, $key='hash') {
		global $afurl;
		if (empty($hash)) return false;
		if (is_array($hash)) {
			if		(!empty($hash[$key]))			$hash = $hash[$key];
			else if	(!empty($hash['thumb_hash']))	$hash = $hash['thumb_hash'];
			else if	(!empty($hash['file_hash']))	$hash = $hash['file_hash'];
			else return false;
		}
		$path = self::cdnPath($hash);
		return empty($path) ? false : $afurl->cdn . '/' . $path;
	}


	public static function cdnAll(&$list, $key='img', $hashkey='hash') {
		if (!is_array($list)) return false;
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


	public function __debugInfo() {
		$dump = [];
		foreach ($this as $key => $item) {
			if (!is_object($item)) $dump[$key] = $item;
		}
		return $dump;
	}


	public $https		= false;
	public $url			= '';
	public $uri			= '';
	public $base		= '';
	public $query		= '';
	public $referer		= '';
	public $domain		= '';
	public $host		= '';
	public $af_host		= '';
	public $part		= '';
	public $jq			= '';
	public $cdn			= '';
	public $static		= '';
	public $img			= '';
	public $upload		= '';
	public $parts		= [];
	public $virtual		= [];
	public $id			= 0;
	public $reparse		= true;
	public $directory	= '';
	private $db			= false;
}

$afurl = new afurl();
