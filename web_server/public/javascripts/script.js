const btn1 = document.getElementById('btn1ID');

const btn2 = document.getElementById('btn2ID');

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

btn1.addEventListener('click', clickHandler1);
btn2.addEventListener('click', clickHandler2);