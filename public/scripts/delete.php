<?php
$rootFolder = '../../'; // Root folder path

function sendRequest($url, $method = 'GET', $data = '') {
    $curl = curl_init();

    // Set the request URL
    curl_setopt($curl, CURLOPT_URL, $url);

    // Set the request method
    curl_setopt($curl, CURLOPT_CUSTOMREQUEST, $method);

    // If the method is POST or DELETE, set the request data
    if ($method == 'POST' || $method == 'DELETE') {
        curl_setopt($curl, CURLOPT_POSTFIELDS, $data);
    }

    // Set the option to receive the response as a string
    curl_setopt($curl, CURLOPT_RETURNTRANSFER, true);

    // Execute the request
    $response = curl_exec($curl);

    // Close the cURL handle
    curl_close($curl);

    return $response;
}

// Function to get the list of items in a folder
function getFolderContents($folderPath)
{
    $contents = scandir($folderPath);
    $files = [];
    $folders = [];

    foreach ($contents as $item) {
        $itemPath = $folderPath . '/' . $item;
        if (is_file($itemPath)) {
            $files[] = $item;
        } elseif (is_dir($itemPath) && $item != '.' && $item != '..') {
            $folders[] = $item;
        }
    }

    return ['files' => $files, 'folders' => $folders];
}

// Check if a folder has been selected
if (isset($_POST['folder'])) {
    $selectedFolder = $_POST['folder'];
    $folderPath = $rootFolder . '/' . $selectedFolder;
    $folderContents = getFolderContents($folderPath);
}

// Check if an item has been selected for deletion
if (isset($_POST['delete'])) {
    $selectedItem = $_POST['delete'];
    $itemPath = $rootFolder . '/' . $selectedFolder . '/' . $selectedItem;

    if (is_file($itemPath)) {
        unlink($itemPath);
    } elseif (is_dir($itemPath)) {
        rmdir($itemPath);
    }

    // Refresh the folder contents after deletion
    $folderContents = getFolderContents($folderPath);
}

// Output HTML
?>

<!DOCTYPE html>
<html>
<head>
    <title>Folder Contents</title>
</head>
<body>
    <h1>Folder Contents</h1>

    <?php if (isset($folderContents)): ?>

        <h3>Select an item to delete:</h3>
        <form method="post" action="">
            <input type="hidden" name="folder" value="<?php echo $selectedFolder; ?>">
            <select name="delete">
                <?php foreach ($folderContents['files'] as $file): ?>
                    <option value="<?php echo $file; ?>"><?php echo $file; ?></option>
                <?php endforeach; ?>
                <?php foreach ($folderContents['folders'] as $folder): ?>
                    <option value="<?php echo $folder; ?>"><?php echo $folder; ?></option>
                <?php endforeach; ?>
            </select>
            <input type="submit" value="Delete">
        </form>

        <h3>Change the folder:</h3>
        <form method="post" action="">
            <p>Enter a new folder name:</p>
            <input type="text" name="folder" placeholder="Folder name">
            <input type="submit" value="Change Folder">
        </form>
    <?php else: ?>
        <form method="post" action="">
            <p>Enter a folder name to view its contents:</p>
            <input type="text" name="folder" placeholder="Folder name">
            <input type="submit" value="Submit">
        </form>
    <?php endif; ?>
</body>
</html>
