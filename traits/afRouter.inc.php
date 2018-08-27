<?php


//TODO:
//		switch afRouter from trait to class
//		do not inherit from afRouter in Altaform anywhere anymore
//		create $afrouter instance of afRouter
//		all internal code routing will be handled by this one class


class afRouter {


	////////////////////////////////////////////////////////////////////////////
	// INITIALIZE AF ROUTER
	////////////////////////////////////////////////////////////////////////////
	public function __construct() {
		$this->directory = getcwd();
	}




	////////////////////////////////////////////////////////////////////////////
	// ROUTE THE HTTP REQUEST
	////////////////////////////////////////////////////////////////////////////
	public function route($af) {
		//RECURSION LIMIT
		static $recurse = 0;
		assert500(
			$recurse++ < 20,
			'INTERNAL REDIRECT RECURSION LIMIT REACHED'
		);

		//RESET VIRTUAL PATHING, IN CASE THIS IS A REPROCESS
		$this->virtual = [];

		//NUMBER OF ITEMS IN URL PATH
		$count = count($this->part) - 1;

		//STORE PATH INFORMATION FOR DEBUGGING
		$this->redirected[] = array_slice($this->part, 1, $count-1);

		//LOAD OUR HOME PAGE!!
		if ($count < 2) {
			return $this->reparse($this->homepage);
		}


		//HANDLE GOOGLE DOMAIN AUTHENTICATION
		if ($count === 2  &&  substr($this->part[1], 0, 6) === 'google') {
			if (!empty($af->config->google['auth'])) {
				$auth = &$af->config->google['auth'];
				switch (true) {
					case is_string($auth)  &&  $this->part[1] === $auth:
					case tbx_array($auth)  &&  in_array($this->part[1], $auth):
						echo 'google-site-verification: ';
						echo $this->part[1];
					return false;
				}
			}
		}


		for ($i=1; $i<$count; $i++) {
			assert400(
				preg_match('/\.(php|inc|hh|tpl)$/i', $this->part[$i]) === 0,
				'Invalid path - possible hacking attempt'
			);

			//FORCE VIRTUAL PATHING IF SPECIAL CHARACTERS ARE FOUND
			//SPECIAL CHARACTER ALLOWED: [SPACE] ! + - . _
			//ALL OTHERS FORCE VITUAL PATHING
			if (!ctype_alnum(substr($this->part[$i], 0, 1)) ||
				preg_match('/[^\x21\x2B\x2D\x2E\x5F 0-9a-zA-Z]/', $this->part[$i])) {

				$this->virtualize($i);

				if (is_dir('_virtual')) {
					$this->chdir('_virtual');
					if ($this->reparse  ||  $this->reparse === NULL) return true;
					if ($count-$i === 1) return $this->index($af);
					continue;
				}

				if (is_file('_virtual.php'))	return '_virtual.php';
				if (is_file('_virtual.hh'))		return $this->hhvm('_virtual.hh');

				error404();
			}


			//IF FRAGMENT IS DIRECTORY, MOVE INTO IT
			if (is_dir($this->part[$i])) {
				$this->chdir( $this->part[$i] );
				if ($this->reparse  ||  $this->reparse === NULL) return true;
				if ($count-$i === 1) return $this->index($af);
				continue;
			}


			//IF WE'RE ON FINAL FRAGMENT, ATTEMPT TO LOAD FILE
			if ($count-$i === 1) {
				$file = $this->part[$i];
				if (is_file($file.'.php'))		return $file.'.php';
				if (is_file($file.'.hh'))		return $this->hhvm($file.'.hh');
				if (is_file($file.'.tpl'))		return $af->auto(true, $file.'.tpl');

				$this->virtualize($i);
				if (is_file('_virtual.php'))	return '_virtual.php';
				if (is_file('_virtual.hh'))		return $this->hhvm('_virtual.hh');
				if (!is_dir('_virtual'))		error404();

				$this->chdir('_virtual');
				if ($this->reparse  ||  $this->reparse === NULL) return true;

				return $this->index($af);
			}


			//NO MATCHES FOUND OTHERWISE FOR FRAGEMENT, VIRTUALIZE INSTEAD
			$this->virtualize($i);


			//ATTEMPT VIRTUAL FOLDER
			if (is_dir('_virtual')) {
				$this->chdir('_virtual');
				if ($this->reparse !== false) return true;
				continue;
			}


			//NO MATCHES FOUND OTHERWISE FOR FRAGEMENT, ATTEMPT VIRTUAL FILE
			if (is_file('_virtual.php'))		return '_virtual.php';
			if (is_file('_virtual.hh'))			return $this->hhvm('_virtual.hh');


			//NO MATCHES FOUND FOR FRAGEMENT, ERROR 404 PAGE!
			error404();
		}
	}




	////////////////////////////////////////////////////////////////////////////
	// REPARSE THE ROUTE
	////////////////////////////////////////////////////////////////////////////
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




	////////////////////////////////////////////////////////////////////////////
	// REPLACE ROUTE
	////////////////////////////////////////////////////////////////////////////
	public function replace($prepend=[], $append=[]) {
		$this->reparse($prepend, $append, true);
	}




	////////////////////////////////////////////////////////////////////////////
	// FINALIZE ROUTE, DISABLE REPARSING
	////////////////////////////////////////////////////////////////////////////
	public function finalize() {
		return ($this->reparse = NULL);
	}




	////////////////////////////////////////////////////////////////////////////
	// MOVE INTO A FOLDER, AND TEST SECURITY IF NEEDED
	////////////////////////////////////////////////////////////////////////////
	private function chdir($__path) {
		assert500(@chdir($__path), 'Unable to enter directory');

		if (!is_file('_altaform.inc.php')) return;

		extract($GLOBALS, EXTR_REFS | EXTR_SKIP);
		require('_altaform.inc.php');

		$__list = get_defined_vars();

		unset($__list['__path']);

		foreach ($__list as $__key => $__value) {
			$GLOBALS[$__key] = $__value;
		}
	}




	////////////////////////////////////////////////////////////////////////////
	// PROCESS INDEX FILE, IF AVAILABLE
	////////////////////////////////////////////////////////////////////////////
	private function index($af) {
		if (is_file('_index.php'))	return '_index.php';
		if (is_file('_index.hh'))	return $this->hhvm('_index.hh');
		if (is_file('_index.tpl'))	return $af->auto(true, '_index.tpl');
		error404();
	}




	////////////////////////////////////////////////////////////////////////////
	// PROCESS A VIRTUAL PATH
	////////////////////////////////////////////////////////////////////////////
	private function virtualize($start) {
		if (!empty($this->virtual)) return;

		$count = count($this->part)-1;
		assert500($start<$count, 'Critical error processing path');

		for ($x=$start; $x<$count; $x++) {
			$this->virtual[] = $this->part[$x];
		}

		$this->id = afString::int($this->virtual[0]);
	}




	////////////////////////////////////////////////////////////////////////////
	// IF $path IS A HACK FILE, THROW AN ERROR WHEN TRYING TO RUN IT ON PHP
	////////////////////////////////////////////////////////////////////////////
	private function hhvm($path) {
		if (defined('HHVM_VERSION')) return $path;
		error500(
			'HHVM_VERSION is not defined. ' .
			'Cannot execute HACK language code from PHP.'
		);
	}




	////////////////////////////////////////////////////////////////////////////
	// MEMBER VARIABLES
	////////////////////////////////////////////////////////////////////////////
	public $id			= 0;
	public $path		= '';
	public $part		= [];
	public $virtual		= [];
	public $redirected	= [];
	public $reparse		= true;
	public $homepage	= 'homepage';
	public $directory	= '';
}
