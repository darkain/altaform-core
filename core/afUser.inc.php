<?php


require_once(__DIR__.'/../traits/afPermission.inc.php');
require_once(__DIR__.'/../traits/afPreference.inc.php');

//TODO:	ADD A TRAIT FOR 'ATTRIBUTES'
//		CURRENTLY ONLY ATTRIBUTE IS UNVERIFIED PASSWORD



class			afUser
	extends		pudlOrm
	implements	afUrlx {
	use			afNode;
	use			afPermission;
	use			afPreference;




	////////////////////////////////////////////////////////////////////////////
	//CONSTRUCTOR
	////////////////////////////////////////////////////////////////////////////
	public function __construct($item=false, $fetch=false) {
		parent::__construct($item, $fetch);

		$this->has_password
			=	!empty($this->user_pass)
			||	!empty($this->auth_password);
	}




	////////////////////////////////////////////////////////////////////////////
	//OVERWRITE THE PUDL PARAMETERS FOR PULLING A COLLECTION
	////////////////////////////////////////////////////////////////////////////
	protected static function schema() {
		return [
			'column'	=> [
				static::prefix.'.*',
				'ua.auth_account',
				'ua.auth_password',
				'ua.auth_verified',
			],

			'table'		=> [static::prefix	=> [static::table,
				['left'	=> ['ua'=>'pudl_user_auth'], 'using'=>'user_id'],
			]],
		];
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
			$this->merge($data);
			return $db->updateId('pudl_user_profile', $data, $this);
		}

		return $this->merge($db->rowId('pudl_user_profile', $this));
	}




	////////////////////////////////////////////////////////////////////////////
	//TRUE IF THE USER IS LOGGED IN, FALSE OTHERWISE
	////////////////////////////////////////////////////////////////////////////
	public function loggedIn() {
		return !!$this->id();
	}




	////////////////////////////////////////////////////////////////////////////
	//GENERATE 401 ERROR PAGE AND EXIT SCRIPT IF USER ISN'T LOGGED IN
	////////////////////////////////////////////////////////////////////////////
	public function requireLogin() {
		assert401($this->loggedIn());
		return $this;
	}




	////////////////////////////////////////////////////////////////////////////
	//GET THE URL PATH OF THE USER
	////////////////////////////////////////////////////////////////////////////
	public function url() {
		return empty($this->user_url) ? $this->id() : $this->user_url;
	}




	////////////////////////////////////////////////////////////////////////////
	//GET THE ICON OF THE USER
	////////////////////////////////////////////////////////////////////////////
	public function image() {
		global $afurl;
		return $afurl->cdn($this, 'thumb_hash', 'mime_id');
	}




	////////////////////////////////////////////////////////////////////////////
	//SET THE USER ACCOUNT PASSWORD
	////////////////////////////////////////////////////////////////////////////
	public function setPassword($account, $password) {
		global $db;

		$this->auth_account = $account;

		return $db->upsert('pudl_user_auth', [
			static::column	=> $this->id(),
			'auth_account'	=> $account,
			'auth_password'	=> password_hash($password, PASSWORD_DEFAULT),
		]);
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




	////////////////////////////////////////////////////////////////////////////
	//GENERATE A RANDOM PASSWORD
	////////////////////////////////////////////////////////////////////////////
	public static function password($length=16) {
		$password	= '';
		$characters	= strlen(static::password_allowed)-1;
		for ($i=0; $i<$length; $i++) {
			$password .= static::password_allowed[random_int(0, $characters)];
		}
		return $password;
	}




	////////////////////////////////////////////////////////////////////////////
	//ADD OR DELETE AN ITEM FROM THE USER'S MESSAGE QUEUE
	////////////////////////////////////////////////////////////////////////////
	function queue($service, $type, $data=false) {
		global $db;

		if ($data === false) {
			$db->delete('pudl_queue', [
				'queue_user'	=> $this->id(),
				'queue_service'	=> $service,
				'queue_type'	=> $type,
			]);
			return;
		}

		if (!tbx_array($data)) $data = [$data];

		$db->insert('pudl_queue', [
				'queue_user'	=> $this->id(),
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
	//LATE STATIC BINDING VARIABLES FROM PUDL ORM
	////////////////////////////////////////////////////////////////////////////
	const	classname	= __CLASS__;
	const	column		= 'user_id';
	const	thumbnail	= 'user_icon';
	const	table		= 'pudl_user';
	const	prefix		= 'us';




	////////////////////////////////////////////////////////////////////////////
	//ALLOWED CHARACTERS IN SYSTEM GENERATED PASSWORDS
	////////////////////////////////////////////////////////////////////////////
	const password_allowed =
		'01234abcdefghijklmnopqrstuvwxyz-^/(' .
		'56789ABCDEFGHIJKLMNOPQRSTUVWXYZ+)\$=';
}




////////////////////////////////////////////////////////////////////////////////
//SHORTCUT CLASS FOR ANONYMOUS USER
////////////////////////////////////////////////////////////////////////////////
class afAnonymous extends afUser {
	public function __construct() {
		parent::__construct(0, true);
	}

	protected function _fetchCache() { return AF_DAY; }
}




////////////////////////////////////////////////////////////////////////////////
//SHORTCUT CLASS FOR A CACHELESS ACCOUNT - USEFUL FOR ADMIN PAGES
////////////////////////////////////////////////////////////////////////////////
class afAccount extends afUser {
	protected function _fetchCache() { return 0; }
}
