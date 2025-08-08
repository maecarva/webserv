#!/usr/bin/php
<?php
$str = $_SERVER['QUERY_STRING'];
$queries = array();
parse_str($_SERVER['QUERY_STRING'], $queries);


$name    = isset($queries['name'])    ? $queries['name']    : '';
$email   = isset($queries['email'])   ? $queries['email']   : '';
$message = isset($queries['message']) ? $queries['message'] : '';


// Génération du corps HTML
$body = <<<HTML
<html>
<body>
<h1>Form Data</h1>
<p>Name: {$name}</p>
<p>Email: {$email}</p>
<p>Message: {$message}</p>
</body>
</html>
HTML;

// En-têtes CGI
printf("Content-Type: text/html\r\n");
printf("Content-Length: %d\r\n\r\n", strlen($body));

// Envoi du corps
echo $body;
?>
