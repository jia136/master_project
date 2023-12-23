const btn1 = document.getElementById('btn1ID');
const btn2 = document.getElementById('btn2ID');

const imageHandler1 = async () => {

    console.log("imageHandler1");
    try {
        const res = await fetch('/image', {method: 'GET'});
        const data = await res.json();
        if (res.ok) {
            const data_str = JSON.stringify(data['data']);
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

function loadTable(data) {
    //const table = document.querySelector('table tbody');
    console.log(data);
    if (data.length == 0) {
        //table.innerHTML = "<tr><td class='no-data' colspan='5'>No Data</td></tr>";
        console.log("data.length == 0");
        return;
    }
    //let images_names = data.split(/(")/u);
    //console.log(images_names);
    /*
    let tableHtml = "";
    data.array.forEach(element => {
        tableHtml += "<tr>"; 
        tableHtml += '<td>${files}</td>';  
        tableHtml += "</tr>";
    });
    /*data.forEach(function ({id, name, date_added}) {
        /*tableHtml += "<tr>";
        tableHtml += `<td>${id}</td>`;
        tableHtml += `<td>${name}</td>`;
        tableHtml += `<td>${new Date(date_added).toLocaleString()}</td>`;
        tableHtml += `<td><button class="delete-row-btn" data-id=${id}>Delete</td>`;
        tableHtml += `<td><button class="edit-row-btn" data-id=${id}>Edit</td>`;
        tableHtml += "</tr>";
    });*/

    //table.innerHTML = tableHtml;
}

btn1.addEventListener('click', clickHandler1);
btn2.addEventListener('click', clickHandler2);
document.addEventListener('DOMContentLoaded', imageHandler1);