<?php


trait af_router {


	////////////////////////////////////////////////////////////////////////////
	// ROUTE THE HTTP REQUEST
	////////////////////////////////////////////////////////////////////////////
	public function route() {
		global $af;

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

			//FORCE VIRTUAL PATHING IF SPECIAL CHARACTERS ARE FOUND
			//SPECIAL CHARACTER ALLOWED: [SPACE] ! + - . _
			//ALL OTHERS FORCE VITUAL PATHING
			if (preg_match('/[^\x21\x2B\x2D\x2E\x5F 0-9a-zA-Z]/', $this->part[$i])) {
				if (is_dir('_virtual')) {
					$this->chdir( $this->virtualize($i, '_virtual') );
					if ($this->reparse  ||  $this->reparse === NULL) return true;
					if ($count-$i === 1) return $this->index();
					continue;
				}

				if (is_file('_virtual.php'))	return $this->virtualize($i, '_virtual.php');
				if (is_file('_virtual.hh'))		return $this->virtualize($i, $this->hhvm('_virtual.hh'));
				error404();
			}


			//IF FRAGMENT IS DIRECTORY, MOVE INTO IT
			if (is_dir($this->part[$i])) {
				$this->chdir( $this->part[$i] );
				if ($this->reparse  ||  $this->reparse === NULL) return true;
				if ($count-$i === 1) return $this->index();
				continue;
			}


			//IF WE'RE ON FINAL FRAGMENT, ATTEMPT TO LOAD FILE
			if ($count-$i === 1) {
				$file = $this->part[$i];
				if (is_file($file.'.php'))		return $file.'.php';
				if (is_file($file.'.hh'))		return $this->hhvm($file.'.hh');
				if (is_file($file.'.tpl'))		return $af->auto(true, $file.'.tpl');
				if (is_file('_virtual.php'))	return $this->virtualize($i, '_virtual.php');
				if (is_file('_virtual.hh'))		return $this->virtualize($i, $this->hhvm('_virtual.hh'));
				if (!is_dir('_virtual'))		error404();

				$this->chdir( $this->virtualize($i, '_virtual') );
				if ($this->reparse  ||  $this->reparse === NULL) return true;

				return $this->index();
			}


			//NO MATCHES FOUND OTHERWISE FOR FRAGEMENT, ATTEMPT VIRTUAL FOLDER
			if (is_dir('_virtual')) {
				$this->chdir( $this->virtualize($i, '_virtual') );
				if ($this->reparse !== false) return true;
				continue;
			}


			//NO MATCHES FOUND OTHERWISE FOR FRAGEMENT, ATTEMPT VIRTUAL FILE
			if (is_file('_virtual.php'))		return $this->virtualize($i, '_virtual.php');
			if (is_file('_virtual.hh'))			return $this->virtualize($i, $this->hhvm('_virtual.hh'));


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




	////////////////////////////////////////////////////////////////////////////
	// PROCESS INDEX FILE, IF AVAILABLE
	////////////////////////////////////////////////////////////////////////////
	private function index() {
		global $af;
		if (is_file('_index.php'))	return '_index.php';
		if (is_file('_index.hh'))	return $this->hhvm('_index.hh');
		if (is_file('_index.tpl'))	return $af->auto(true, '_index.tpl');
		error404();
	}




	////////////////////////////////////////////////////////////////////////////
	// PROCESS A VIRTUAL PATH
	////////////////////////////////////////////////////////////////////////////
	private function virtualize($start, $return=false) {
		if (!empty($this->virtual)) return $return;

		$count = count($this->part)-1;
		assert500($start<$count, 'Critical error processing path');

		for ($x=$start; $x<$count; $x++) {
			$this->virtual[] = $this->part[$x];
		}

		$this->id = afstring::int($this->virtual[0]);

		return $return;
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
	public $part		= [];
	public $virtual		= [];
	public $redirected	= [];
	public $reparse		= true;
	public $homepage	= 'homepage';
}
