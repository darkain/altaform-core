# Every [thing] Is An Island

## Every PATH Is An Island

With the Altaform Router, each URL path is treated like an island, a code path
entirely independent from any other URL's code. This means when you load the
page `/one/`, the code for `/two/` is never even loaded, let alone parsed or
executed.

The Altaform Router works nearly identical to a classic file system based
router, such as a traditional Apache + PHP installation. When you request the
URL `/one/`, the Altaform Router will attempt to load the local file
`/one/_index.php`.
