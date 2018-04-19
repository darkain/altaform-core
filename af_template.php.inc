<?php


define('AF_STAGE_NONE',		0);
define('AF_STAGE_HEADER',	1);
define('AF_STAGE_BODY',		2);
define('AF_STAGE_FOOTER',	3);
define('AF_STAGE_COMPLETE',	4);



trait af_template {


	////////////////////////////////////////////////////////////////////////////
	//LINK A JAVASCRIPT FILE IN HTML HEADER
	////////////////////////////////////////////////////////////////////////////
	public function script($path) {
		if (tbx_array($path)) {
			foreach ($path as $item) {
				$this->_script[] = ['path' => $item];
			}
		} else {
			$this->_script[] = ['path' => $path];
		}
		return $this;
	}




	////////////////////////////////////////////////////////////////////////////
	//EMBED INLINE JAVASCRIPT IN HTML HEADER
	////////////////////////////////////////////////////////////////////////////
	public function javascript($script, $merge=false) {
		$this->_js[] = [$script, $merge];
		return $this;
	}




	////////////////////////////////////////////////////////////////////////////
	//EMBED INLINE JAVASCRIPT IN HTML HEADER FROM FILE
	////////////////////////////////////////////////////////////////////////////
	public function js($file, $merge=false) {
		$data = @file_get_contents($file, true);
		if ($data === false) {
			throw new afException('Unable to load JS file: '.$file);
		}
		$this->_js[] = [$data, $merge];
		return $this;
	}




	////////////////////////////////////////////////////////////////////////////
	//LINK A CASCADING STYLE SHEET FILE IN HTML HEADER
	////////////////////////////////////////////////////////////////////////////
	public function style($path) {
		if (tbx_array($path)) {
			foreach ($path as $item) {
				$this->_style[] = ['path' => $item];
			}
		} else {
			$this->_style[] = ['path' => $path];
		}
		return $this;
	}




	////////////////////////////////////////////////////////////////////////////
	//EMBED INLINE CASCADING STYLE SHEET IN HTML HEADER
	////////////////////////////////////////////////////////////////////////////
	public function sheet($styles, $merge=false) {
		$this->_css[] = [$styles, $merge];
		return $this;
	}




	////////////////////////////////////////////////////////////////////////////
	//EMBED INLINE CASCADING STYLE SHEET IN HTML HEADER FROM FILE
	////////////////////////////////////////////////////////////////////////////
	public function css($file, $merge=false) {
		$data = @file_get_contents($file, true);
		if ($data === false) {
			throw new afException('Unable to load CSS file: '.$file);
		}
		$this->_css[] = [$data, $merge];
		return $this;
	}




	////////////////////////////////////////////////////////////////////////////
	//ADD EXTRA CUSTOM META TAGS TO A PAGE (SUCH AS TWITTER CARDS)
	////////////////////////////////////////////////////////////////////////////
	public function meta($meta) {
		$this->_meta[] = $meta;
		return $this;
	}




	////////////////////////////////////////////////////////////////////////////
	//ADD EXTRA CUSTOM META TAGS TO A PAGE (SUCH AS TWITTER CARDS)
	////////////////////////////////////////////////////////////////////////////
	public function metas($meta) {
		$this->_meta = array_merge($this->_meta, $meta);
		return $this;
	}




	////////////////////////////////////////////////////////////////////////////
	//LOAD A TEMPLATE FILE - OVERRIDE DEFAULT TBX FILE LOADER
	////////////////////////////////////////////////////////////////////////////
	public function onload(&$text) {
		global $afurl, $og, $user;

		$this('af',		$this,	$text);
		$this('afurl',	$afurl,	$text);
		$this('og',		$og,	$text);
		$this('user',	$user,	$text);

		return parent::onload($text);
	}




	////////////////////////////////////////////////////////////////////////////
	//LOAD A TEMPLATE FILE - OVERRIDE DEFAULT TBX FILE LOADER
	////////////////////////////////////////////////////////////////////////////
	public function load($file) {
		if (tbx_array($file)) {
			return $this->loadArray($file);
		}

		$device	= $file . '.' . afdevice::device();
		if (af_fs::readfile($device)) return parent::load($device);

		$pathed	= $this->path() . $device;
		if (af_fs::readfile($pathed)) return parent::load($pathed);

		return parent::load($file);
	}




	////////////////////////////////////////////////////////////////////////////
	//NO PARAMS - RENDER HTML HEADER TEMPLATES
	//PARAMS - ADD OPTIONS TO HTML HEADER TEMPLATES (deprecated)
	////////////////////////////////////////////////////////////////////////////
	public function header($key=false, $data=false, $replace=false) {
		if ($key === false) {
			if ($this->cli()) return $this;
			if (!$this->jq()) return $this->headerHTML()->headerPage();

			list($js, $css) = $this->prerender();
			if ($js  !== '') echo '<script>\'use strict\';' . $js . '</script>';
			if ($css !== '') echo '<style>' . $css . '</style>';

		} else if ($replace  ||  empty($this->_headers[$key])) {
			$this->_headers[$key]	= [$data];

		} else {
			$this->_headers[$key][]	= $data;
		}

		return $this;
	}




	////////////////////////////////////////////////////////////////////////////
	//SET A SPECIFIC HTML HEADER TEMPLATE OPTION
	////////////////////////////////////////////////////////////////////////////
	public function headers($key, $data) {
		$this->_headers[$key] = $data;
		return $this;
	}




	////////////////////////////////////////////////////////////////////////////
	//NO PARAMS - RENDER HTML FOOTER TEMPLATES
	//PARAMS - ADD OPTIONS TO HTML FOOTER TEMPLATES (deprecated)
	////////////////////////////////////////////////////////////////////////////
	public function footer($key=false, $data=false, $replace=false) {
		if ($key === false) {
			if ($this->jq() || $this->cli()) return $this;
			return $this->footerPage()->footerHTML();
		}

		if ($replace  ||  empty($this->_footers[$key])) {
			$this->_footers[$key]	= [$data];
		} else {
			$this->_footers[$key][]	= $data;
		}

		return $this;
	}




	////////////////////////////////////////////////////////////////////////////
	//SET A SPECIFIC HTML FOOTER TEMPLATE OPTION
	////////////////////////////////////////////////////////////////////////////
	public function footers($key, $data) {
		$this->_footers[$key] = $data;
		return $this;
	}




	////////////////////////////////////////////////////////////////////////////
	//RENDER HTML HEADER
	////////////////////////////////////////////////////////////////////////////
	public function headerHTML() {
		if ($this->_stage !== AF_STAGE_NONE) return $this;
		$this->_stage = AF_STAGE_HEADER;

		$device	= afdevice::device();
		$root	= $this->path() . $this->config->root;


		//NOTE: PRERENDER MUST COME BEFORE LOAD, SINCE IT LOADS/RENDERS TOO
		list ($js, $css) = $this->prerender();


		if ($this->debug()  &&  af_fs::readfile($root.'/header_html_debug.tpl.'.$device)) {
			$this->load($root.'/header_html_debug.tpl.'.$device);
		} else if ($this->debug()  &&  af_fs::readfile($root.'/header_html_debug.tpl')) {
			$this->load($root.'/header_html_debug.tpl');
		} else {
			$this->load($root.'/header_html.tpl');
		}

		return $this
			->field('js',		$js)
			->field('css',		$css)
			->field('robots',	implode(', ', $this->_robots))
			->block('script',	$this->_script)
			->block('style',	$this->_style)
			->block('meta',		$this->_meta)
			->render();
	}




	////////////////////////////////////////////////////////////////////////////
	//RENDER PAGE HEADER
	////////////////////////////////////////////////////////////////////////////
	public function headerPage() {
		if ($this->_stage !== AF_STAGE_HEADER) return $this;

		$device	= afdevice::device();
		$root	= $this->path() . $this->config->root;

		if ($this->debug()  &&  af_fs::readfile($root.'/header_page_debug.tpl.'.$device)) {
			$this->load($root.'/header_page_debug.tpl.'.$device);
		} else if ($this->debug()  &&  af_fs::readfile($root.'/header_page_debug.tpl')) {
			$this->load($root.'/header_page_debug.tpl');
		} else {
			$this->load($root.'/header_page.tpl');
		}

		$this->merge($this->_headers)->render();

		$this->_stage = AF_STAGE_BODY;

		return $this;
	}




	////////////////////////////////////////////////////////////////////////////
	//RENDER PAGE HEADER
	////////////////////////////////////////////////////////////////////////////
	public function headerEmail() {
		$root	= $this->path() . $this->config->root;

		if ($this->debug()  &&  af_fs::readfile($root.'/header_email_debug.tpl')) {
			$this->load($root.'/header_email_debug.tpl');
		} else {
			$this->load($root.'/header_email.tpl');
		}

		return $this->renderToString();
	}




	////////////////////////////////////////////////////////////////////////////
	//RENDER HTML FOOTER
	////////////////////////////////////////////////////////////////////////////
	public function footerHTML() {
		if ($this->_stage !== AF_STAGE_FOOTER) return $this;

		$device	= afdevice::device();
		$root	= $this->path() . $this->config->root;
		$ok		= false;

		if ($this->debug()) {
			if (af_fs::readfile($root.'/footer_html_debug.tpl.'.$device)) {
				$ok = $this->render($root.'/footer_html_debug.tpl.'.$device);
			} else if (af_fs::readfile($root.'/footer_html_debug.tpl')) {
				$ok = $this->render($root.'/footer_html_debug.tpl');
			}
		}

		if (!$ok) $this->render($root.'/footer_html.tpl');

		$this->_stage = AF_STAGE_COMPLETE;

		return $this;
	}




	////////////////////////////////////////////////////////////////////////////
	//RENDER PAGE FOOTER
	////////////////////////////////////////////////////////////////////////////
	public function footerPage() {
		global $db, $user;

		if ($this->_stage !== AF_STAGE_BODY) return $this;
		$this->_stage = AF_STAGE_FOOTER;

		$device	= afdevice::device();
		$root	= $this->path() . $this->config->root;

		if ($this->debug()  &&  af_fs::readfile($root.'/footer_page_debug.tpl.'.$device)) {
			$this->load($root.'/footer_page_debug.tpl.'.$device);
		} else if ($this->debug()  &&  af_fs::readfile($root.'/footer_page_debug.tpl')) {
			$this->load($root.'/footer_page_debug.tpl');
		} else {
			$this->load($root.'/footer_page.tpl');
		}

		$this->merge($this->_footers)->render();

		if ($user->isAdmin()  ||  ($user->isStaff()  &&  $this->debug())) {
			if (af_fs::readfile($root.'/footer_admin.tpl')) {
				$this->load($root.'/footer_admin.tpl');
				if (!empty($db)) {
					$this->field('dbstats',		$db->stats());
					$this->block('dbmisses',	$db->stats()['missed']);
				}
				$this->render();
			}
		}

		return $this;
	}




	////////////////////////////////////////////////////////////////////////////
	//RENDER HTML FOOTER
	////////////////////////////////////////////////////////////////////////////
	public function footerEmail() {
		$root	= $this->path() . $this->config->root;

		if ($this->debug()  &&  af_fs::readfile($root.'/footer_email_debug.tpl')) {
			$this->load($root.'/footer_email_debug.tpl');
		} else {
			$this->load($root.'/footer_email.tpl');
		}

		return $this->renderToString();
	}




	////////////////////////////////////////////////////////////////////////////
	//RENDER AN ENTIRE PAGE
	////////////////////////////////////////////////////////////////////////////
	public function renderPage($file, $merge=false) {
		global $og, $user;

		//ALLOW DEVICE SPECIFIC LOADING
		$device	= $file . '.' . afdevice::device();

		//PULL THE CONTENTS OF THE TEMPLATE BEFORE ANYTHING ELSE!
		$text = @file_get_contents( af_fs::readfile($device) ? $device : $file );
		if ($text === false) {
			throw new afException('Unable to load template file: '.$file);
		}

		//PULL OUT REQUIRED PERMISSIONS TAG
		preg_match("/\<permission[^>]*>(.*)\<\/permission\>/", $text, $matches);
		if (!empty($matches[1])) {
			if ($matches[1] === 'login') {
				$user->requireLogin();
			} else {
				$user->requirePermission($matches[1]);
			}
			$text = preg_replace("/\<permission[^>]*>(.*)\<\/permission\>/", '', $text);
		}

		//PULL OUT TITLE TAG
		preg_match("/\<title[^>]*>(.*)\<\/title\>/", $text, $matches);
		if (!empty($matches[1])) {
			$this->title	= $this->renderString($matches[1]);
			$text			= preg_replace("/\<title[^>]*>(.*)\<\/title\>/", '', $text);
		}

		//PULL OUT STYLE TAG
		if (!$this->jq()) {
			preg_match("/\<style[^>]*>(.*)\<\/style\>/s", $text, $matches);
			if (!empty($matches[1])) {
				$this->sheet($this->renderString($matches[1]));
				$text	= preg_replace("/\<style[^>]*>(.*)\<\/style\>/s", '', $text);
			}
		}

		//PULL OUT DESCRIPTION TAG
		preg_match("/\<description[^>]*>(.*)\<\/description\>/s", $text, $matches);
		if (!empty($matches[1])) {
			$og['description'] = $this->renderString($matches[1]);
			$text	= preg_replace("/\<description[^>]*>(.*)\<\/description\>/s", '', $text);
		}

		//PULL OUT IMAGE TAG
		preg_match("/\<image[^>]*>(.*)\<\/image\>/", $text, $matches);
		if (!empty($matches[1])) {
			$og['image'] = $this->renderString($matches[1]);
			$text	= preg_replace("/\<image[^>]*>(.*)\<\/image\>/", '', $text);

			if (!empty($og['twittername'])  &&  !empty($og['twitterdomain'])) {
				$this->metas([
					['name'=>'twitter:card',		'content'=>'photo'],
					['name'=>'twitter:site',		'content'=>$og['twittername']],
					['name'=>'twitter:domain',		'content'=>$og['twitterdomain']],
					['name'=>'twitter:title',		'content'=>$this->title],
					['name'=>'twitter:image',		'content'=>$og['image']],
					['name'=>'twitter:description',	'content'=>$og['description']],
				]);
			}
		}

		//RENDER ALL THE THINGS!
		return $this
			->header()
				->loadString($text)
					->merge($merge)
				->render()
			->footer();
	}




	////////////////////////////////////////////////////////////////////////////
	//AUTOMATICALLY INCLUDE JS AND CSS FILES IF AVAILABLE, AND RENDER OUTPUT
	////////////////////////////////////////////////////////////////////////////
	public function auto($render=false, $file=false) {
		if ($file === false) {
			$file = debug_backtrace(DEBUG_BACKTRACE_IGNORE_ARGS, 2)[0]['file'];
		}

		$info	= pathinfo($file);
		$name	= '/' . substr($info['basename'], 0, -3);
		$ext	= substr($info['basename'], -4);

		if (!in_array($ext, ['.php', '.tpl'])) {
			throw new afException(
				'Invalid file extension, only ".php" or ".tpl" are supported: '
				. $info['basename']
			);
		}

		if (af_fs::readfile($info['dirname'].$name.'js')) {
			$this->js($info['dirname'].$name.'js');
		}

		if (af_fs::readfile($info['dirname'].$name.'css')) {
			$this->css($info['dirname'].$name.'css');
		}

		if ($render) $this->renderPage($info['dirname'].$name.'tpl');

		return $this;
	}




	////////////////////////////////////////////////////////////////////////////
	//GET THE CURRENT PAGE RENDERING STAGE
	////////////////////////////////////////////////////////////////////////////
	public function stage() { return $this->_stage; }




	////////////////////////////////////////////////////////////////////////////
	//RESET THE CURRENT STAGE - USED BY ERROR HANGLER
	////////////////////////////////////////////////////////////////////////////
	public function resetStage() {
		$this->_stage = AF_STAGE_NONE;
		return $this;
	}




	////////////////////////////////////////////////////////////////////////////
	//SET A CUSTOM NOTICE HEADER
	////////////////////////////////////////////////////////////////////////////
	public function notice($text) {
		$this->_headers['notice'][] = ['text' => $text];
		return $this;
	}




	////////////////////////////////////////////////////////////////////////////
	//PRE-RENDER JAVASCRIPT AND CSS FOR HTML HEADER
	////////////////////////////////////////////////////////////////////////////
	protected function prerender() {
		$return = ['', ''];

		//PRE-RENDER JAVASCRIPT
		foreach ($this->_js as $item) {
			$return[0] .= $this	->loadString($item[0])
								->merge($item[1])
								->renderToString();
		}

		//PRE-RENDER CASCADING STYLE SHEETS
		foreach ($this->_css as $item) {
			$return[1] .= $this	->loadString($item[0])
								->merge($item[1])
								->renderToString();
		}

		return $return;
	}




	private $_js		= [];
	private $_css		= [];
	private $_script	= [];
	private $_style		= [];
	private $_meta		= [];
	private $_headers	= [];
	private $_footers	= [];
	private $_stage		= AF_STAGE_NONE;

}
