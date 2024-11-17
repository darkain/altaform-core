<?php



require_once('orm.php');




class			afo_gallery
	extends		afo_orm {
	use			afo_template;




	////////////////////////////////////////////////////////////////////////////
	// URL TO THIS OBJECT
	////////////////////////////////////////////////////////////////////////////
	public function url() {
		global $afurl;
		//TODO: we should use a "base" URL here instead of hard-coding the "user profile"
		return $afurl->user(
			$this,
			'gallery',
			$this->gallery_id
		);
	}




	////////////////////////////////////////////////////////////////////////////
	// OVERWRITE THE PUDL PARAMETERS FOR PULLING A COLLECTION
	////////////////////////////////////////////////////////////////////////////
	protected static function schema() {
		return array_merge_recursive(parent::schema(), [
			'column'	=> ['us.*'],

			'table'		=> ['us' => 'pudl_user'],

			'clause'	=> [
//				af_filter_banned('us'),
				'us.user_id' => pudl::column([static::prefix,'user_id']),
			],
		]);
	}



	////////////////////////////////////////////////////////////////////////////
	// LATE STATIC BINDING VARIABLES FROM PUDL ORM
	////////////////////////////////////////////////////////////////////////////
	const	name		= 'gallery_name';
	const	column		= 'gallery_id';
	const	icon		= 'ga.gallery_thumb';
	const	table		= 'pudl_gallery';
	const	prefix		= 'ga';

}
