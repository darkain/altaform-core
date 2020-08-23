<?php


////////////////////////////////////////////////////////////////////////////////
// IMPORT REQUIRED MODULES
////////////////////////////////////////////////////////////////////////////////
\af\module('node');
\af\module('permission');
\af\module('preference');



//TODO:	ADD A TRAIT FOR 'ATTRIBUTES'
//		CURRENTLY ONLY ATTRIBUTE IS UNVERIFIED PASSWORD




////////////////////////////////////////////////////////////////////////////////
// HANDLES AN INSTANCE OF A SINGLE USER
////////////////////////////////////////////////////////////////////////////////
class			afUser
	extends		\pudlOrm
	implements	afUrlx {
	use			\af\node;
	use			\af\permission;
	use			\af\preference;




	////////////////////////////////////////////////////////////////////////////
	// CONSTRUCTOR
	////////////////////////////////////////////////////////////////////////////
	public function __construct(\pudl $pudl=NULL, $item=false, $fetch=false) {
		parent::__construct($pudl, $item, $fetch);

		$this->has_password
			=	!empty($this->user_pass)
			||	!empty($this->auth_password);
	}




	////////////////////////////////////////////////////////////////////////////
	// OVERWRITE THE PUDL PARAMETERS FOR PULLING A COLLECTION
	////////////////////////////////////////////////////////////////////////////
	protected static function schema() {
		return [
			'column'	=> [
				static::prefix.'.*',
				'ua.auth_account',
				'ua.auth_password',
				'ua.auth_verified',
			],

			//TODO:	WE SHOULD NOT BE PULLING IN AUTHENTICATION TABLE HERE.
			//		AUTHENTICATION TABLE SHOULD ONLY BE PULLED FOR LOGIN/SESSION INFO ONLY.
			'table'		=> [static::prefix	=> [static::table,
				['left'	=> ['ua'=>'user_auth'], 'using'=>'user_id'],
			]],
		];
	}




	////////////////////////////////////////////////////////////////////////////
	// UPDATE THE USER TABLE IN THE DATABASE
	////////////////////////////////////////////////////////////////////////////
	public function update($data) {
		global $af;
		if (!$this->loggedIn()) return false;
		$return = parent::update($data);
		$af->purgeSession(false, $this);
		return $return;
	}




	////////////////////////////////////////////////////////////////////////////
	// LOAD OR UPDATE THE PROFILE TABLE IN THE DATABASE
	////////////////////////////////////////////////////////////////////////////
	public function profile($data=false) {
		if (!$this->loggedIn()) return false;

		if ($data !== false) {
			$this->merge($data);
			return $this->pudl()->updateId('user_profile', $data, $this);
		}

		return $this->merge($this->pudl()->rowId('user_profile', $this));
	}




	////////////////////////////////////////////////////////////////////////////
	// TRUE IF THE USER IS LOGGED IN, FALSE OTHERWISE
	////////////////////////////////////////////////////////////////////////////
	public function loggedIn() {
		return !!$this->id();
	}




	////////////////////////////////////////////////////////////////////////////
	// GENERATE 401 ERROR PAGE AND EXIT SCRIPT IF USER ISN'T LOGGED IN
	////////////////////////////////////////////////////////////////////////////
	public function requireLogin($code=401) {
		\af\affirm($code, $this->loggedIn());
		return $this;
	}




	////////////////////////////////////////////////////////////////////////////
	// GET THE URL PATH OF THE USER
	////////////////////////////////////////////////////////////////////////////
	public function url() {
		return empty($this->user_url) ? $this->id() : $this->user_url;
	}




	////////////////////////////////////////////////////////////////////////////
	// GET THE ICON OF THE USER
	////////////////////////////////////////////////////////////////////////////
	public function image() {
		global $afurl;
		return $afurl->cdn($this, 'thumb_hash', 'mime_id');
	}




	////////////////////////////////////////////////////////////////////////////
	// SET THE USER ACCOUNT PASSWORD
	////////////////////////////////////////////////////////////////////////////
	public function setPassword($account, $password, $algo=PASSWORD_DEFAULT) {
		$this->auth_account = $account;

		return $this->pudl()->upsert('user_auth', [
			static::column	=> $this->id(),
			'auth_account'	=> $account,
			'auth_password'	=> password_hash($password, $algo),
		]);
	}




	////////////////////////////////////////////////////////////////////////////
	// VALIDATE A GIVEN PASSWORD BASED ON CONFIGURATION DATA
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

		if (!empty($afconfig->password['upper'])) {
			if (!preg_match('/[a-z]/', $password)  ||  !preg_match('/[A-Z]/', $password)) {
				return 'Password must contain both upper and lower case letters';
			}
		}

		if (!empty($afconfig->password['number'])) {
			if (!preg_match('/[0-9]/', $password)) {
				return 'Password must contain numbers';
			}
		}

		if (!empty($afconfig->password['symbol'])) {
			if (!preg_match('/[^0-9a-zA-Z]/', $password)) {
				return 'Password must contain special character symbols';
			}
		}

		return true;
	}




	////////////////////////////////////////////////////////////////////////////
	// GENERATE A RANDOM PASSWORD
	////////////////////////////////////////////////////////////////////////////
	public static function password($length=16) {
		$allowed	= static::password_allowed;
		$password	= '';
		$characters	= strlen($allowed)-1;
		for ($i=0; $i<$length; $i++) {
			$password .= $allowed[random_int(0, $characters)];
		}
		return $password;
	}




	////////////////////////////////////////////////////////////////////////////
	// ADD OR DELETE AN ITEM FROM THE USER'S MESSAGE QUEUE
	////////////////////////////////////////////////////////////////////////////
	function queue($service, $type, $data=false) {
		if ($data === false) {
			$this->pudl()->delete('queue', [
				'queue_user'	=> $this->id(),
				'queue_service'	=> $service,
				'queue_type'	=> $type,
			]);
			return;
		}

		if (!tbx_array($data)) $data = [$data];

		$this->pudl()->insert('queue', [
				'queue_user'	=> $this->id(),
				'queue_service'	=> $service,
				'queue_type'	=> $type,
				'queue_time'	=> $this->pudl()->time(),
				'queue_message'	=> $data,
			], [
				'queue_time'	=> $this->pudl()->time(),
				'queue_message'	=> $data,
				'queue_count'	=> pudl::_increment(1),
			]
		);
	}




	////////////////////////////////////////////////////////////////////////////
	// HOW LONG SHOULD THE FETCHED DATA BE CACHED FOR (IN SECONDS)
	////////////////////////////////////////////////////////////////////////////
	protected function _fetchCache() { return AF_HOUR; }




	////////////////////////////////////////////////////////////////////////////
	// LATE STATIC BINDING VARIABLES FROM PUDL ORM
	////////////////////////////////////////////////////////////////////////////
	const	column		= 'user_id';
	const	icon		= 'user_icon';
	const	json		= 'user_json';
	const	table		= 'user';
	const	prefix		= 'us';




	////////////////////////////////////////////////////////////////////////////
	// ALLOWED CHARACTERS IN SYSTEM GENERATED PASSWORDS
	////////////////////////////////////////////////////////////////////////////
	const password_allowed =
		'01234abcdefghijklmnopqrstuvwxyz-^/(56789ABCDEFGHIJKLMNOPQRSTUVWXYZ+)\$=';
}




////////////////////////////////////////////////////////////////////////////////
// SHORTCUT CLASS FOR ANONYMOUS USER
////////////////////////////////////////////////////////////////////////////////
class afAnonymous extends afUser {
	public function __construct(\pudl $pudl) {
		try {
			parent::__construct($pudl, 0, true);

		} catch (\pudlException $e) {
			$this->user_id		= 0;
			$this->user_name	= 'anonymous';
			$this->user_json	= [];
			$this->user_icon	= NULL;
		}
	}

	protected function _fetchCache() { return AF_DAY; }
}




////////////////////////////////////////////////////////////////////////////////
//SHORTCUT CLASS FOR A CACHELESS ACCOUNT - USEFUL FOR ADMIN PAGES
////////////////////////////////////////////////////////////////////////////////
class afAccount extends afUser {
	protected function _fetchCache() { return 0; }
}
