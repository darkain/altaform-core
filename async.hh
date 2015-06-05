<?hh

async function afrun(array<Awaitable<string>> $handles): Awaitable<array<string>> {
	await AwaitAllWaitHandle::fromArray($handles);
	return array_map($handle ==> $handle->result(), $handles);
}
