<?php


namespace af;




////////////////////////////////////////////////////////////////////////////////
// HANDLES USER AUTHENTICATION
////////////////////////////////////////////////////////////////////////////////
trait auth {


	////////////////////////////////////////////////////////////////////////////
	// INTERFACE - CALLBACK AFTER SESSION IS PROCESSED (EVERY PAGE REQUEST)
	////////////////////////////////////////////////////////////////////////////
	public function postLogin() {}




	////////////////////////////////////////////////////////////////////////////
	// PROCESS SESSION INFORMATION (EVERY PAGE REQUEST)
	////////////////////////////////////////////////////////////////////////////
	public function login($session=NULL) {
		global $user, $get;

		if (empty($this->pudl)) {
			$user = new \afUser($this->pudl);
			return $user;
		}

		$user = NULL;

		if (empty($session)) $session = session_id();

		if (!empty($session)) {
			if ($id = (int) $get->session('AF:USER_ID')) {
				$data = $this->pudl->cache(AF_HOUR, 'AF-SESSION-'.$session)->row(
					[\afUser::prefix => \afUser::icon()],
					[
						'user_id' => $id,
						\pudl::find('user_permission', $this->_authtype),
					]
				);
				if (!empty($data)) $user = new \afUser($this->pudl, $data);
			}
		}

		if (empty($user)) $user = new \afAnonymous($this->pudl);

		if (empty($user->user_url)) $user->user_url = $user->user_id;

		$user->user_session = $session;

		$this->authenticate($user, false);
		$user->permissions();
		$this->postLogin();
		return $user;
	}




	////////////////////////////////////////////////////////////////////////////
	// LOG THE CURRENT USER OUT (DESTOY CURRENT USER'S SESSION)
	////////////////////////////////////////////////////////////////////////////
	public function logout($session=false, $destroy=true) {
		global $user;

		if (empty($session)) $session = session_id();

		$this->authenticate(0, $destroy);

		$user = new \afAnonymous($this->pudl);
		$user->permissions();

		if (!$destroy) return;

		if (ini_get('session.use_cookies')) {
			$params = session_get_cookie_params();
			setcookie(session_name(), '', $this->time() - AF_YEAR,
				$params['path'],	$params['domain'],
				$params['secure'],	$params['httponly']
			);
		}

		session_unset();
		session_destroy();
	}




	////////////////////////////////////////////////////////////////////////////
	// DESTROY ALL INSTANCES OF THIS SESSION ACROSS DATABASE CLUSTER AND REDIS
	////////////////////////////////////////////////////////////////////////////
	public function purgeSession($session=false, $account=false) {
		global $user;

		if ($account === false) {
			$account = $user;
			if (empty($session)) $session = session_id();
			$this->pudl->sync()->purge('AF-SESSION-'.$session);
		}

		if (empty($account['user_id'])) return;

		$this->pudl->uncache()->rowId('user', 'user_id', (int)$account['user_id']);

		$rows = $this->pudl->selectRows(
			'id',
			$this->_session->table(),
			['user' => $account['user_id']]
		);

		foreach ($rows as $item) {
			$this->pudl->purge('AF-SESSION-'.$item['id']);
		}
	}




	////////////////////////////////////////////////////////////////////////////
	// LOG THE CURRENT SESSION INTO THE GIVEN USER ACCOUNT (CALLED ONCE)
	////////////////////////////////////////////////////////////////////////////
	public function authenticate($user, $purge=true) {
		if ($this->_session) {
			if (tbx_array($user)) $user = $user['user_id'];
			$this->_session->user($user, 'AF:USER_ID');
		}
		if ($purge) $this->purgeSession();
	}




	////////////////////////////////////////////////////////////////////////////
	// STORES THE PUDLSESSION CLASS INSTANCE
	////////////////////////////////////////////////////////////////////////////
	private	$_session	= NULL;




	////////////////////////////////////////////////////////////////////////////
	// ACCEPTABLE ACCOUNT TYPES FOR LOGIN
	////////////////////////////////////////////////////////////////////////////
	public	$_authtype	= ['user', 'staff', 'admin'];

}
