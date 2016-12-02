<?php


require_once('af_node.php.inc');
require_once('af_permission.php.inc');
require_once('af_preference.php.inc');
require_once('db_object.php.inc');

//TODO:	ADD A TRAIT FOR 'ATTRIBUTES'
//		CURRENTLY ONLY ATTRIBUTE IS UNVERIFIED PASSWORD



class			afuser
	extends		db_object
	implements	af_url {
	use			af_node,	af_permission,		af_preference;




	////////////////////////////////////////////////////////////////////////////
	//PASS THE TABLE, COLUMN, AND ITEM INFO TO DB_OBJECT CONSTRUCTOR
	////////////////////////////////////////////////////////////////////////////
	public function __construct($item=false, $fetch=false) {
		parent::__construct('pudl_user', 'user_id', $item, $fetch);
	}




	////////////////////////////////////////////////////////////////////////////
	//UPDATE THE USER TABLE IN THE DATABASE
	////////////////////////////////////////////////////////////////////////////
	public function update($data) {
		global $af;
		if (!$this->loggedIn()) return false;
		$return = parent::update($data);
		$af->purgeSession(false, $this);
		return $return;
	}




	////////////////////////////////////////////////////////////////////////////
	//LOAD OR UPDATE THE PROFILE TABLE IN THE DATABASE
	////////////////////////////////////////////////////////////////////////////
	public function profile($data=false) {
		global $db;
		if (!$this->loggedIn()) return false;

		if ($data !== false) {
			return $db->updateId('pudl_user_profile', $data, $this);
		}

		$this->_merge($db->rowId('pudl_user_profile', $this));
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
	//GET THE URL OF THE CURRENT USER
	////////////////////////////////////////////////////////////////////////////
	public function url() {
		return empty($this->user_url) ? $this->user_id : $this->user_url;
	}




	////////////////////////////////////////////////////////////////////////////
	//SET THE USER ACCOUNT PASSWORD
	////////////////////////////////////////////////////////////////////////////
	public function setPassword($account, $password) {
		global $db;

		$this->auth_account = $account;

		return $db->insert('pudl_user_auth', [
			'user_id'		=> $this->user_id,
			'auth_account'	=> $account,
			'auth_password'	=> password_hash($password, PASSWORD_DEFAULT),
		], true);
	}




	////////////////////////////////////////////////////////////////////////////
	//VALIDATE A GIVEN PASSWORD BASED ON CONFIGURATION DATA
	////////////////////////////////////////////////////////////////////////////
	public static function validatePassword($password) {
		global $afconfig;

		if (!is_string($password)) {
			return 'Invalid password format';
		}

		if (empty($password)) {
			return 'No password specified';
		}

		if (strlen($password) < $afconfig->password['length']) {
			return 'Password must be at least '
				. $afconfig->password['length']
				. ' characters long';
		}

		if ($afconfig->password['upper']) {
			if (!preg_match('/[a-z]/', $password)  ||  !preg_match('/[A-Z]/', $password)) {
				return 'Password must contain both upper and lower case letters';
			}
		}

		if ($afconfig->password['number']) {
			if (!preg_match('/[0-9]/', $password)) {
				return 'Password must contain numbers';
			}
		}

		if ($afconfig->password['symbol']) {
			if (!preg_match('/[^0-9a-zA-Z]/', $password)) {
				return 'Password must contain special character symbols';
			}
		}

		return true;
	}

//public static generate password



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
	//HOW LONG SHOULD THE FETCHED DATA BE CACHED FOR (IN SECONDS)
	////////////////////////////////////////////////////////////////////////////
	protected function _fetchCache() { return AF_HOUR; }




	////////////////////////////////////////////////////////////////////////////
	//CREATE NEW USER ACCOUNT WITH THE GIVEN DATA
	////////////////////////////////////////////////////////////////////////////
	public static function create($data=false) {
		global $db;
		return self::get($db->insert('pudl_user', $data));
	}




	////////////////////////////////////////////////////////////////////////////
	//GET A USER ACCOUNT FROM DATABASE FOR A GIVEN ID NUMBER
	////////////////////////////////////////////////////////////////////////////
	public static function get($id) {
		return new afuser($id, true);
	}




	////////////////////////////////////////////////////////////////////////////
	//GET A USER ACCOUNT FROM DATABASE FOR A GIVEN ID NUMBER
	////////////////////////////////////////////////////////////////////////////
	public static function select($clause) {
		global $db;
		return new afuser($db->row('pudl_user', $clause));
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
