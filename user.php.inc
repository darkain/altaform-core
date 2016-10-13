<?php

require_once('af_node.php.inc');
require_once('af_object_array.php.inc');
require_once('af_permission.php.inc');
require_once('af_preference.php.inc');

//TODO:	ADD A TRAIT FOR 'ATTRIBUTES'
//		CURRENTLY ONLY ATTRIBUTE IS UNVERIFIED PASSWORD

if (!interface_exists('pudlId')) {
	interface afuser_interface {}
} else {
	interface afuser_interface extends pudlId {}
}



class afuser extends af_object_array implements afuser_interface, af_url {
	use af_node;
	use af_permission;
	use af_preference;




	public function __construct($item=false, $fetch=false) {
		if (is_array($item)) {
			$fetch ? $this->fetch($item)		: $this->_replace($item);

		} else if (is_a($item, 'pudlResult')) {
			$fetch ? $this->fetch($item())		: $this->_clone($item());

		} else if (is_a($item, 'afurl')) {
			$fetch ? $this->fetch($item->id)	: $this->_clone(['user_id'=>$item->id]);

		} else if (is_a($item, 'Traversable')) {
			$fetch ? $this->fetch($item)		: $this->_clone($item);

		} else if ($fetch  &&  (is_int($item)  ||  ctype_digit($item))) {
			$this->fetch($item);

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
		$af->purgeSession(false, $this);
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
	public function is($item) {
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
	public function pudlId($column=true) {
		if ($column === true) return ['user_id' => $this->user_id];
		return $this->user_id;
	}




	////////////////////////////////////////////////////////////////////////////
	//GET THE URL OF THE CURRENT USER
	////////////////////////////////////////////////////////////////////////////
	public function url() {
		return empty($this->user_url) ? $this->user_id : $this->user_url;
	}




	////////////////////////////////////////////////////////////////////////////
	//SHORTCUT FUNCTIONS FOR ERROR CHECKING
	////////////////////////////////////////////////////////////////////////////
	public function assert401() { assert401($this->user_id); return $this; }
	public function assert404() { assert404($this->user_id); return $this; }




	////////////////////////////////////////////////////////////////////////////
	//SET THE USER ACCOUNT PASSWORD
	////////////////////////////////////////////////////////////////////////////
	public function setPassword($account, $password) {
		global $db;
		$db->insert('pudl_user_auth', [
			'user_id'		=> $this->user_id,
			'auth_account'	=> $account,
			'auth_password'	=> password_hash($password, PASSWORD_DEFAULT),
		], true);
	}




	////////////////////////////////////////////////////////////////////////////
	//ADD OR DELETE AN ITEM FROM THE USER'S MESSAGE QUEUE
	////////////////////////////////////////////////////////////////////////////
	function queue($service, $type, $data=false) {
		global $db;

		if ($data === false) {
			$db->delete('pudl_queue', [
				'queue_user'	=> $this->user_id,
				'queue_service'	=> $service,
				'queue_type'	=> $type,
			]);
			return;
		}

		if (!tbx_array($data)) $data = [$data];

		$db->insert('pudl_queue', [
				'queue_user'	=> $this->user_id,
				'queue_service'	=> $service,
				'queue_type'	=> $type,
				'queue_time'	=> $db->time(),
				'queue_message'	=> $data,
			], [
				'queue_time'	=> $db->time(),
				'queue_message'	=> $data,
				'queue_count'	=> pudl::_increment(1),
			]
		);
	}




	////////////////////////////////////////////////////////////////////////////
	//CREATE NEW USER ACCOUNT WITH GIVEN DATA
	////////////////////////////////////////////////////////////////////////////
	public static function create($data) {
		global $db;
		return new afuser($db->insert('pudl_user', $data), true);
	}




	////////////////////////////////////////////////////////////////////////////
	//FETCH DATA FROM DATABASE
	////////////////////////////////////////////////////////////////////////////
	protected function fetch($id) {
		global $db;

		if (is_array($id))	$id = $id['user_id'];
		if (is_object($id))	$id = $id->user_id;

		$this->_clone(
			$db->cache(AF_HOUR)->rowId('pudl_user', 'user_id', (int)$id)
		);
	}
}




////////////////////////////////////////////////////////////////////////////////
//SHORTCUT CLASS FOR AUTOMATIC ERROR HANDLING, THROWS ERROR 401 ON UNKNOWN USER
////////////////////////////////////////////////////////////////////////////////
class afuser401 extends afuser {
	public function __construct($id) {
		parent::__construct($id, true);
		$this->assert401();
	}
}




////////////////////////////////////////////////////////////////////////////////
//SHORTCUT CLASS FOR AUTOMATIC ERROR HANDLING, THROWS ERROR 404 ON UNKNOWN USER
////////////////////////////////////////////////////////////////////////////////
class afuser404 extends afuser {
	public function __construct($id) {
		parent::__construct($id, true);
		$this->assert404();
	}
}




////////////////////////////////////////////////////////////////////////////////
//SHORTCUT CLASS FOR ANONYMOUS USER
////////////////////////////////////////////////////////////////////////////////
class afanonymous extends afuser {
	public function __construct() {
		parent::__construct(0, true);
	}
}
