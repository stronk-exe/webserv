<?php
if ($_SERVER['REQUEST_METHOD'] === 'PUT') {
    // Read the input data from the request body
    $inputData = file_get_contents('php://input');

    // Perform the necessary processing with the input data
    // For example, you can save it to a file or update a database record

    // Return a response
    header('Content-Type: text/plain');
    echo 'PUT request processed successfully.';
} else {
    header('HTTP/1.1 405 Method Not Allowed');
    header('Allow: PUT');
    echo 'Only the PUT method is allowed for this resource.';
}
?>
