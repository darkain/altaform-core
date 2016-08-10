<?php


//TODO: PLACE THIS IS DEDICATED AFUSER CLASS


trait af_permission {


	public function hasPermission($permission, $account=false) {
		global $user, $db;

		if (!is_array($permission)) $permission = explode(',', $permission);
		if (empty($account)) $account = &$user;

		if (is_int($account)  ||  is_string($account)) {
			$account = $db->rowId('pudl_user', 'user_id', $account);
			if (empty($account)) return false;
		}

		if (empty($account['permission'])) $this->permissions($account);

		foreach ($permission as $perm) {
			$perm = trim($perm);
			if (!empty($account['permission'][$perm])) return true;
		}

		return false;
	}




	public function requirePermission($permission, $account=false) {
		assert401(
			$this->hasPermission($permission, $account),
			'This page requires the following permission level: ' .
				(is_array($permission) ? implode(', ', $permission) : $permission)
		);
		return $this;
	}




	public function permissions(&$account) {
		global $site;

		//USER ACCESS RIGHTS
		$account['permission'] = $site['permission'];

		if (empty($account['user_permission'])) {
			$account['user_permission'] = 'guest';
		}

		$permissions = explode(',', $account['user_permission']);
		foreach ($permissions as $val) {
			$account['permission'][$val] = 1;
		}

		if (!empty($account['user_adfree'])) {
			if ($account['user_adfree'] > $this->time()) {
				$account['permission']['adfree'] = 1;
			}
		}
	}


}
