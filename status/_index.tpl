<style>
#pagebody {width:auto; min-width:990px;}
</style>

<main class="af-default">
	<h1 class="af-header">[var.title]</h1>

	<table class="tablesorter-blue">
		<thead>
			<tr>
				<th>#</th>
				<th>Path</th>
				<th>Version</th>
				<th>Boot</th>
				<th>Uptime</th>
				<th>Memory</th>
				<th>Delay</th>
			</tr>
		</thead>
		<tbody>
			<tr>
				<td class="right">[server.#;block=tr]</td>
				<td><a target="_blank" href="[server.path]">[server.path]</a></td>
				<td class="right">
					[server.version;noerr]
					<span> - [server.type;noerr;magnet=span]</span>
					<span> - [server.arch;noerr;magnet=span]</span>
				</td>
				<td class="nobr center">[server.boot;noerr;date=Y-m-d @ h:i:s]</td>
				<td class="nobr center">[server.uptime;noerr]</td>
				<td class="nobr right">[server.memory;noerr]</td>
				<td class="right">[server.delay;noerr]</td>
			</tr>
		</tbody>
	</table>

</main>


<script>$(function(){ $('.tablesorter-blue').tablesorter(); });</script>
