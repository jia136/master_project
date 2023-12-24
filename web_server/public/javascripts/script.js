const btn1 = document.getElementById('btn1ID');
const btn2 = document.getElementById('btn2ID');

const imageHandler1 = async () => {

    console.log("imageHandler1");
    try {
        const res = await fetch('/image', {method: 'GET'});
        const data = await res.json();
        if (res.ok) {
            const data_str = data['data'];
            loadTable(data_str);
            return;    
        }
        throw new Error('Request faild.');
    } catch (error) {
        console.log(error);    
    }
    
};

const clickHandler1 = async () => {

    console.log('button was clicked');

    try {
        const res = await fetch('/clicked', {method: 'POST'});
        const data = await res.json();
        if (res.ok) {
            document.getElementById('paraID').innerHTML = JSON.stringify(data);
            return;    
        }
        throw new Error('Request faild.');
    } catch (error) {
        console.log(error);    
    }

};

const clickHandler2 = async () => {

    console.log('button2 was clicked');
    const selectElement = document.getElementById('log_level');
    const output = selectElement.value;
    try {
        const res = await fetch('/loglevel', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify({ esp_log_level: output })
        });
        const data = await res.json();
        if (res.ok) {
            console.log('OK!');
            
            console.log(data);
            return;    
        }
        throw new Error('Request faild.');
    } catch (error) {
        console.log(error);    
    }

};

async function viewFunction(image_download) {

    try {
        const res = await fetch('/temp', {
            method: 'POST',
            headers: {
                //'Content-Type': 'image/jpg'
                'Content-Type':'application/json'
            },
            body: JSON.stringify({ name: image_download })
        });
        
        if (res.ok) {
            const data = await res.blob();
            console.log(data)
            var binaryData = [];
            binaryData.push(data);
            var blobObj = new Blob((binaryData), { type: "image/jpg" });
            var url = window.URL.createObjectURL(blobObj);
            console.log(url);
            document.getElementById('image').setAttribute("src", url);   
            return;
        }
        throw new Error('Request faild.');

    } catch (error) {
        console.log(error);    
    }
}

/*async function viewFunction(image_view) {
    console.log(image_view);
}*/

function loadTable(data) {

    const table = document.querySelector('table tbody');
    let tableHtml = "";

    if (data.length == 0) {
        tableHtml += "<tr>";
        tableHtml += `<td>"[no data]"</td>`;
        tableHtml += `<td>"[no data]]"</td>`;
        tableHtml += `<td>"-"</td>`;
        tableHtml += `<td>"-"</td>`;
        tableHtml += "</tr>";
        console.log("data.length == 0");
        return;
    }
    const obj = JSON.parse(data)
    
    obj.forEach(element => {
        console.log(element);
        var created_time = element.image_time;
        created_time = created_time.replace("T", " ");
        created_time = created_time.replace("Z", " ");
        tableHtml += "<tr>";
        tableHtml += `<td>${element.image_name}</td>`;
        tableHtml += `<td>${element.image_size} bytes</td>`;
        tableHtml += `<td><button class="view-row-btn" data-id=${element.image_name} onclick="viewFunction('${element.image_name}')">View</td>`;
        tableHtml += `<td>${created_time}</td>`;
        tableHtml += "</tr>";
    });

    table.innerHTML = tableHtml;

}

btn1.addEventListener('click', clickHandler1);
btn2.addEventListener('click', clickHandler2);
document.addEventListener('DOMContentLoaded', imageHandler1);