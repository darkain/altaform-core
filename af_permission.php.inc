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


}
