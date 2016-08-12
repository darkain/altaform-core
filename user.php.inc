<?php

require_once('af_object_array.php.inc');
require_once('af_permission.php.inc');
require_once('af_preference.php.inc');



class afuser extends af_object_array {
	use af_permission;
	use af_preference;




	public function __construct($item=false, $fetch=false) {
		global $db;

		if (is_array($item)) {
			$this->_replace($item);

		} else if (is_a($item, 'Iterable')) {
			$this->_clone($item());

		} else if (is_a($item, 'pudlResult')) {
			$this->_clone($item());

		} else if ($fetch  &&  !empty($item)) {
			$this->_clone(
				$db->cache(AF_HOUR)->rowId('pudl_user', 'user_id', $item)
			);

		} else if (!empty($item)) {
			$this->user_id = $item;
		}


		//SET DEFAULT USER ID
		if (empty($this->user_id)) $this->user_id = 0;
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

}
