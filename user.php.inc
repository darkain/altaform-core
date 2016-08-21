<?php

require_once('af_node.php.inc');
require_once('af_object_array.php.inc');
require_once('af_permission.php.inc');
require_once('af_preference.php.inc');



class afuser extends af_object_array {
	use af_node;
	use af_permission;
	use af_preference;




	public function __construct($item=false, $fetch=false) {
		global $db;

		if (is_array($item)) {
			$this->_replace($item);

		} else if (is_a($item, 'pudlResult')) {
			$this->_clone($item());

		} else if (is_a($item, 'Traversable')) {
			$this->_clone($item);

		} else if ($fetch  &&  (is_int($item)  ||  ctype_digit($item))) {
			$this->_clone(
				$db->cache(AF_HOUR)->rowId('pudl_user', 'user_id', (int)$item)
			);

		} else if (!empty($item)) {
			$this->user_id = $item;
		}


		//SET DEFAULT USER ID AND FORCE INTEGER
		$this->user_id = (empty($this->user_id)) ? 0 : (int) $this->user_id;
	}




	////////////////////////////////////////////////////////////////////////////
	//IF CONVERTING CLASS TO STRING, RETURNS THE CURRENT USER ID NUMBER
	////////////////////////////////////////////////////////////////////////////
	public function __toString() {
		return (string) $this->user_id;
	}




	////////////////////////////////////////////////////////////////////////////
	//UPDATE THE USER TABLE IN THE DATABASE
	////////////////////////////////////////////////////////////////////////////
	public function update($data) {
		global $af, $db;
		if (!$this->loggedIn()) return false;
		$return = $db->updateId('pudl_user', $data, 'user_id', $this);
		$af->purgeSession();
		return $return;
	}




	////////////////////////////////////////////////////////////////////////////
	//UPDATE THE PROFILE TABLE IN THE DATABASE
	////////////////////////////////////////////////////////////////////////////
	public function profile($data) {
		global $db;
		if (!$this->loggedIn()) return false;
		return $db->updateId('pudl_user_profile', $data, 'user_id', $this);
	}




	////////////////////////////////////////////////////////////////////////////
	//TRUE IF THE USER IS LOGGED IN, FALSE OTHERWISE
	////////////////////////////////////////////////////////////////////////////
	public function loggedIn() {
		return !empty($this->user_id);
	}




	////////////////////////////////////////////////////////////////////////////
	//GENERATE 401 ERROR PAGE AND EXIT SCRIPT IF USER ISN'T LOGGED IN
	////////////////////////////////////////////////////////////////////////////
	public function requireLogin() {
		assert401($this->loggedIn());
		return $this;
	}




	////////////////////////////////////////////////////////////////////////////
	//COMPARE ITEM TO SEE IF IT IS THE CURRENT USER
	////////////////////////////////////////////////////////////////////////////
	function is($item) {
		if (empty($item))		return false;
		if (is_array($item))	$item = $item['user_id'];
		if (is_object($item))	$item = $item->user_id;
		$item = (int) $item;
		if ($item === 0)		return false;
		return $this->user_id === $item;
	}




	////////////////////////////////////////////////////////////////////////////
	//PUDL INTEGRATION
	////////////////////////////////////////////////////////////////////////////
	public function pudl_getId($column=true) {
		if ($column === true) return ['user_id' => $this->user_id];
		return $this->user_id;
	}




	////////////////////////////////////////////////////////////////////////////
	//GET THE URL OF THE CURRENT USER
	////////////////////////////////////////////////////////////////////////////
	function url() {
		return empty($this->user_url) ? $this->user_id : $this->user_url;
	}

}
