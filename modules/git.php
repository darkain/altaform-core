<?php

namespace af;



require_once(is_owner(__DIR__.'/abyss.php'));




////////////////////////////////////////////////////////////////////////////////
// A SIMPLE CLASS FOR HANDLING COMMON GIT TASKS
////////////////////////////////////////////////////////////////////////////////
class		git
	extends	abyss {




	////////////////////////////////////////////////////////////////////////////
	// CONSTRUCTOR - TAKE IN AN INSTANCE OF ALTAFORM CLASS AND STORE IT LOCALLY
	////////////////////////////////////////////////////////////////////////////
	public function __construct($path) {
		$this->path = $path;
	}




	////////////////////////////////////////////////////////////////////////////
	// ALLOW MEMBER FUNCTIONS TO BE ACCESSED AS IF THEY WERE A VARIABLE
	////////////////////////////////////////////////////////////////////////////
	public function __get($name) {
		if (empty($name)) return NULL;
		if ($name[0] === '_') return NULL;
		if (!method_exists($this, $name)) return NULL;
		return call_user_func([$this, $name]);
	}




	////////////////////////////////////////////////////////////////////////////
	// EXECUTE THE GIVEN GIT COMMAND, AND RETURN THE CONSOLE OUTPUT AS A STRING
	////////////////////////////////////////////////////////////////////////////
	public function __invoke() {
		$command	= func_get_arg(0);
		$path		= getcwd();
		chdir($this->path);
		$return		= `git {$command}`;
		chdir($path);
		return $return;
	}




	////////////////////////////////////////////////////////////////////////////
	// GET THE UNIX TIMESTAMP OF THE MOST RECENT COMMIT
	////////////////////////////////////////////////////////////////////////////
	public function timestamp() {
		return strtotime( $this('show -s --format=%ci') );
	}




	////////////////////////////////////////////////////////////////////////////
	// GET THE SUMMARY OF THE MOST RECENT COMMIT
	////////////////////////////////////////////////////////////////////////////
	public function summary() {
		return $this('show -s');
	}




	////////////////////////////////////////////////////////////////////////////
	// LOCAL VARIABLES
	////////////////////////////////////////////////////////////////////////////
	private $path;
}