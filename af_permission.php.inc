<?php


trait af_permission {


	public function hasPermission($permission) {
		if (empty($this->permission)) $this->permissions();

		if (!tbx_array($permission)) {
			$permission = explode(',', $permission);
		}

		foreach ($permission as $perm) {
			$perm = trim($perm);
			if (!empty($this->permission[$perm])) return true;
		}

		return false;
	}



	public function isAdmin() { return $this->hasPermission('admin'); }
	public function isStaff() { return $this->hasPermission(['staff','admin']); }



	public function requirePermission($permission) {
		assert401(
			$this->hasPermission($permission),
			'This page requires the following permission level: '
			. (tbx_array($permission) ? implode(', ', $permission) : $permission)
		);
		return $this;
	}




	public function requireAdmin() { return $this->requirePermission('admin'); }
	public function requireStaff() { return $this->requirePermission(['staff','admin']); }




	public function permissions() {
		global $af, $site;

		//USER ACCESS RIGHTS
		$this->permission = $site['permission'];

		if (empty($this->user_permission)) {
			$this->user_permission = 'guest';
		}

		$permissions = explode(',', $this->user_permission);
		foreach ($permissions as $val) {
			$val = trim($val);
			$this->permission[$val] = 1;
		}

		if (!empty($this->user_adfree)) {
			if ($this->user_adfree > $af->time()) {
				$this->permission['adfree'] = 1;
			}
		}

		return $this;
	}




	public function hasAccess($access) {
		if (empty($this->access)) $this->access();

		if (!tbx_array($access)) {
			$access = explode(',', $access);
			foreach ($access as &$item) $item = trim($item);
		}

		foreach ($access as $item) {
			if (!empty($this->access[$item])) return true;
		}

		return false;
	}




	public function requireAccess($access) {
		assert401(
			$this->hasAccess($access),
			'This page requires the following permission level: '
			. (tbx_array($access) ? implode(', ', $access) : $access)
		);
		return $this;
	}




	public function access() {
		global $db;

		if (isset($this->access)) return $this;

		$this->access	= [];
		$access			= $db->rowsId('pudl_user_access', $this);

		foreach ($access as $item) {
			$this->access[ $item['user_access'] ] = true;
		}

		return $this;
	}




	public function hasAccessPermission($access, $permission) {
		return $this->hasAccess($access)
			|| $this->hasPermission($permission);
	}




	public function hasAccessAdmin($access) {
		return $this->hasAccessPermission($access, 'admin');
	}




	public function hasAccessStaff($access) {
		return $this->hasAccessPermission($access, ['staff','admin']);
	}




	public function requireAccessPermission($access, $permission) {
		assert401(
			$this->hasAccessPermission($access, $permission),
			'This page requires the following permission level: '
			. (tbx_array($access)		? implode(', ', $access)		: $access)
			. ', '
			. (tbx_array($permission)	? implode(', ', $permission)	: $permission)
		);
		return $this;
	}




	public function requireAccessAdmin($access) {
		return $this->requireAccessPermission($access, 'admin');
	}




	public function requireAccessStaff($access) {
		return $this->requireAccessPermission($access, ['staff','admin']);
	}

}
