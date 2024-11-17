<script>
$(function(){
	var filter_search	= $('#prometheus-filter').val();
	var filter_timer	= null;
	var filter_jq		= null;

	var filter_tags = function() {
		var search = $(this).val();
		if (filter_search === search) return;
		filter_search = search;

		if (filter_jq)		filter_jq.abort();
		if (filter_timer)	clearTimeout(filter_timer);

		filter_timer = setTimeout(function(){
			history.replaceState(
				{},
				[af.title;safe=json],
				[afurl.full;safe=json] + '?search=' + encodeURIComponent(search)
			);

			filter_jq = $.get(
				[afurl.search;ifempty=[afurl.full];safe=json],
				{jq:1, search:search},
				function(data) {
					data = $(data);

					if (data.has('#prometheus-filter-result')) {
						data = data.children()
					}

					$('#prometheus-filter-result').html(data);
				}
			);
		}, 200);
	}

	$('#prometheus-filter')
		.change(filter_tags)
		.keydown(filter_tags)
		.keyup(filter_tags)
		.mousedown(filter_tags)
		.mouseup(filter_tags)
		.focus(filter_tags)
		.blur(filter_tags);
});
</script>

<div class="center">
	<input type="text" id="prometheus-filter"
		value="[search]" placeholder="Search [af.title]" />
</div>
