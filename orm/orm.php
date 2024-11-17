<?php



require_once('_pudl/pudlOrm.php');
require_once('collection.php');
require_once('template.php');




////////////////////////////////////////////////////////////////////////////////
// ALTAFORM PROMETHEUS ORM
////////////////////////////////////////////////////////////////////////////////
abstract class	afo_orm
	extends		pudlOrm {




	////////////////////////////////////////////////////////////////////////////
	// CONSTRUCTOR
	////////////////////////////////////////////////////////////////////////////
	public function __construct(pudl $pudl, $item=false, $fetch=false) {
		parent::__construct($pudl, $item, $fetch);
		$this->id = $this->id();
	}




	////////////////////////////////////////////////////////////////////////////
	// URL TO THIS OBJECT
	////////////////////////////////////////////////////////////////////////////
	abstract public function url();




	////////////////////////////////////////////////////////////////////////////
	// OVERWRITE THE PUDL PARAMETERS FOR PULLING A COLLECTION
	////////////////////////////////////////////////////////////////////////////
	protected static function schema() {
		$return = [
			'column'	=> [static::prefix.'.*', 'th.thumb_hash'],

			'table'		=> [static::prefix	=> [static::table,
				['left' => ['th'=>'pudl_file_thumb'], 'clause' => [
						'th.file_hash'	=> pudl::column([static::icon]),
						'th.thumb_type'	=> static::thumb(),
				]]
			]]
		];

		if (static::table !== 'pudl_file') {
			$return['column'][] = 'fl.file_width';
			$return['column'][] = 'fl.file_height';
			$return['column'][] = 'fl.file_average';
			$return['column'][] = 'fl.mime_id';

			$return['table'][static::prefix][] = [
				'left'=>['fl'=>'pudl_file'], 'on'=>'th.file_hash=fl.file_hash'
			];
		}

		return $return;
	}




	////////////////////////////////////////////////////////////////////////////
	// PROCESS PROMETHEUS STYLE FORMATTING
	////////////////////////////////////////////////////////////////////////////
	public function prometheus($size=300) {
		global $afurl;

		// DONT DOUBLE-PROCESS ITEMS
		if (!empty($this->prometheus)) return;
		$this->prometheus = true;


		// CHECK IF DATA IS AVAILABLE TO CALCULATE APSECT RATIO
		if (empty($this->file_width)  ||  empty($this->file_height)) {
			$this->ratio = 1;

		} else {
			$this->ratio = $this->file_width / $this->file_height;
		}


		// CALCULATE PROMETHEUS IMAGE WIDTH FROM IMAGE ASPECT RATIO
		$this->width = (int) ($size * min(max($this->ratio, 0.3), 1.5));


		// SET PROMETHEUS NAME
		if (empty($this->name)) {
			$this->name = '';
			if (static::name !== false  &&  !empty($this->{static::name})) {
				$this->name = $this->{static::name};
			}
		}


		// URL FOR THE IMAGE FILE ITSELF
		// TODO: WE NEED A DIFFERENT DEFAULT IMAGE - THIS IS COSPIX SPECIFIC
		if (empty($this->thumb_hash)) {
			$this->class	= 'cpn-discover-tag';
			$this->img		= $afurl->static . '/thumb2/'
							. substr(get_class($this), 4)
							. '.svg';
		} else {
			$this->img = $afurl->cdn($this, 'thumb_hash', 'mime_id');
		}


		// URL FOR THE IMAGE'S PAGE
		$this->url = $this->url();


		// CHAIN THIS METHOD
		return $this;
	}




	////////////////////////////////////////////////////////////////////////////
	// SIZE OF THUMBNAIL
	// NOTE: YES, THIS IS INTENTIONALLY A STRING, NOT AN INTEGER
	////////////////////////////////////////////////////////////////////////////
	public static function thumb() {
		return '500';
	}




	////////////////////////////////////////////////////////////////////////////
	// SET THE ITEM NAME TO THE USER'S NAME
	////////////////////////////////////////////////////////////////////////////
	public function username() {
		$this->name = $this->user_name;
	}




	////////////////////////////////////////////////////////////////////////////
	// SET THE ITEM NAME TO THE GALLERY'S NAME
	////////////////////////////////////////////////////////////////////////////
	public function galleryname() {
		$this->name = $this->gallery_name;
	}




	////////////////////////////////////////////////////////////////////////////
	// RENDER
	////////////////////////////////////////////////////////////////////////////
	public function render($af) {
		if (!$this->prometheus) $this->prometheus();

		if (static::$template === false) {
			$af->load('_altaform/templates/main.tpl');
			static::$template = (string) $af;
		}

		$af->Source = static::$template;
		$af->field('item', $this)->render();

		return $this;
	}




	////////////////////////////////////////////////////////////////////////////
	// HEADER
	////////////////////////////////////////////////////////////////////////////
	public static function header($af, $collection, $item=false, $data='') {
		if (static::$header === false) {
			$af->load('_altaform/templates/header.tpl');
			static::$header = (string) $af;
		}

		$af->Source = static::$header;
		if ($item) $af->field('item', $item);
		$af->field('data', $data);
		$af->field('collection', $collection);
		$af->render();
	}




	////////////////////////////////////////////////////////////////////////////
	// FOOTER
	////////////////////////////////////////////////////////////////////////////
	public static function footer($af, $collection) {
		if (static::$footer === false) {
			$af->load('_altaform/templates/footer.tpl');
			static::$footer = (string) $af;
		}

		$af->Source = static::$footer;
		$af->field('collection', $collection);
		$af->render();
	}




	////////////////////////////////////////////////////////////////////////////
	// LOCAL VARIABLES
	////////////////////////////////////////////////////////////////////////////
	public $id = 0;




	////////////////////////////////////////////////////////////////////////////
	// LATE STATIC BINDING VARIABLES
	////////////////////////////////////////////////////////////////////////////
	const collector	= 'afo_collection';
	const name		= false;

}
