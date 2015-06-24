<?php

require_once('closure.php.inc');


class afurl extends afclosure {
	public function construct($db=false) {
		global $get, $site;

		//Configurable paths
		if (!empty($site['base']))		$this->base		= $site['base'];
		if (!empty($site['cdn']))		$this->cdn		= $site['cdn'];
		if (!empty($site['static']))	$this->static	= $site['static'];
		if (!empty($site['upload']))	$this->upload	= $site['upload'];
		if (!empty($site['img']))		$this->img		= $site['img'];
		if (!empty($site['file']))		$this->file		= $site['file'];

		$this->uri		= urldecode($get->server('REQUEST_URI', '', _GETVAR_BASIC));
		$this->domain	= $get->server('HTTP_HOST', '', _GETVAR_BASIC);
		$this->referer	= $get->server('HTTP_REFERER', '', _GETVAR_BASIC);
		$this->https	= strtolower($get->server('HTTPS'));
		$this->https	= ($this->https !== 'off'  &&  !empty($this->https));
		$this->host		= ($this->https?'https://':'http://') . $this->domain;
		$this->af_host	= $this->host;

		$this->parts	= (array)parse_url($this->uri);
		if (empty($this->parts['path'])) error404();

		if (substr($this->parts['path'],  0, 1) !== '/') $this->parts['path']  = '/' . $this->parts['path'];
		if (substr($this->parts['path'], -1, 1) !== '/') $this->parts['path'] .= '/';
		if (substr($this->parts['path'],  1, 1) === '_') error500();

		$this->part = explode('/', $this->parts['path']);
		$this->url = '/';
		foreach ($this->part as $key => $val) {
			$this->safe[] = !empty($db) ? $db->safe($val) : @mysql_real_escape_string($val);
			if (strlen($val) < 1) continue;
			$this->url .= urlencode($val) . '/';
		}

		$this->all	= $this->host . $this->base . $this->url;
		$this->full	= $this->all;
		if (!empty($this->parts['query'])  &&  $this->parts['query']!=='jq=1') {
			$this->all .= '?' . $this->parts['query'];
		}
	}


	public function vid($new_id=false) {
		if ($new_id !== false) $this->id = afstring::int($new_id);
		return $this->id;
	}


	public function user($user) {
		return $this->base . '/' . empty($user['user_url']) ? $user['user_id'] : $user['user_url'] . '/';
	}


	public function process() {
		global $site, $af;

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
			$this->virtual[] = $this->safe[$x];
		}

		$this->vid($this->virtual[0]);
	}


	public $https		= false;
	public $url			= '';
	public $uri			= '';
	public $base		= '';
	public $referer		= '';
	public $domain		= '';
	public $host		= '';
	public $af_host		= '';
	public $part		= '';
	public $jq			= '';
	public $cdn			= '';
	public $static		= '';
	public $img			= '';
	public $file		= '';
	public $upload		= '';
	public $parts		= [];
	public $safe		= [];
	public $virtual		= [];
	public $id			= 0;
}

$afurl = new afurl();
