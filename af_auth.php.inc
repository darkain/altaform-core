<?php


trait af_auth {


	////////////////////////////////////////////////////////////////////////////
	//INTERFACE - CALLBACK AFTER SESSION IS PROCESSED (EVERY PAGE REQUEST)
	////////////////////////////////////////////////////////////////////////////
	public function postLogin() {}




	////////////////////////////////////////////////////////////////////////////
	//PROCESS SESSION INFORMATION (EVERY PAGE REQUEST)
	////////////////////////////////////////////////////////////////////////////
	public function login($session=false) {
		global $user, $db, $get;

		if (empty($db)) return $user = new afuser();

		$user = false;

		if (empty($session)) $session = session_id();

		if (!empty($session)) {
			if ($id = (int) $get->session('AF:USER_ID')) {
				$data = $db->cache(AF_HOUR, 'AF-SESSION-'.$session)->row([
					'us' => _pudl_user()
				], [
					'user_id'			=> $id,
					'user_permission'	=> pudl::inSet($this->_authtype),
				]);
				if (!empty($data)) $user = new afuser($data);
			}
		}

		if (empty($user)) {
			$data = $db->cache(AF_HOUR)->rowId('pudl_user', 'user_id', 0);
			if (!empty($data)) $user = new afuser($data);
		}

		if (empty($user)) $user = new afuser();

		$user->user_session = $session;
		if (empty($user->user_url)) $user->user_url = $user->user_id;

		$this->authenticate($user, false);
		$user->permissions();
		$this->postLogin();
		return $user;
	}




	////////////////////////////////////////////////////////////////////////////
	//LOG THE CURRENT USER OUT (DESTOY CURRENT USER'S SESSION)
	////////////////////////////////////////////////////////////////////////////
	public function logout($session=false, $destroy=true) {
		global $user, $db;

		if (empty($session)) $session = session_id();

		$this->authenticate(0, $destroy);

		$user = new afuser($db->rowId('pudl_user', 'user_id', 0));
		$user->permissions($user);

		if (!$destroy) return;

		if (ini_get('session.use_cookies')) {
			$params = session_get_cookie_params();
			setcookie(session_name(), '', $this->time() - 42000,
				$params['path'],	$params['domain'],
				$params['secure'],	$params['httponly']
			);
		}

		session_unset();
		session_destroy();
	}




	////////////////////////////////////////////////////////////////////////////
	//DESTROY ALL INSTANCES OF THIS SESSION ACROSS DATABASE CLUSTER AND REDIS
	////////////////////////////////////////////////////////////////////////////
	public function purgeSession($session=false) {
		global $db, $user;

		if (empty($session)) $session = session_id();
		$db->sync()->purge('AF-SESSION-'.$session);

		if (empty($user['user_id'])) return;

		$rows = $db->selectRows(
			'id',
			$this->_session->table(),
			['user' => $user['user_id']]
		);

		foreach ($rows as $item) {
			$db->purge('AF-SESSION-'.$item['id']);
		}
	}




	////////////////////////////////////////////////////////////////////////////
	//LOG THE CURRENT SESSION INTO THE GIVEN USER ACCOUNT (CALLED ONCE)
	////////////////////////////////////////////////////////////////////////////
	public function authenticate($user, $purge=true) {
		if (tbx_array($user)) $user = $user['user_id'];
		$this->_session->user($user, 'AF:USER_ID');
		if ($purge) $this->purgeSession();
	}




	////////////////////////////////////////////////////////////////////////////
	//STORES THE PUDLSESSION CLASS INSTANCE
	////////////////////////////////////////////////////////////////////////////
	private	$_session	= false;




	////////////////////////////////////////////////////////////////////////////
	//ACCEPTABLE ACCOUNT TYPES FOR LOGIN
	////////////////////////////////////////////////////////////////////////////
	public	$_authtype	= ['user','staff','admin'];

}
