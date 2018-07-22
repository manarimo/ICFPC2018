function waitUntilSuccess() {
    const stdout = document.getElementById('stdout');
    return new Promise((resolve, reject) => {
        const id = setInterval(() => {
            if (stdout.innerText.includes('Success')) {
                clearInterval(id);
                resolve();
            } else if (stdout.innerText.includes('Failure')) {
                clearInterval(id);
                reject();
            }
        }, 500);
    });
}

document.addEventListener('DOMContentLoaded', () => {
    const base = window.location.host === '' ? 'http://localhost:8081' : 'http://nanachi.kadingel.osak.jp';
    const button = document.getElementById('execTrace');
    button.addEventListener('click', async () => {
        await waitUntilSuccess();
        const energy = stdout.innerText.match(/Energy:\s+(\d+)/)[1];
        const srcModelIn = document.getElementById('srcModelFileIn');
        const modelIn = document.getElementById('tgtModelFileIn');
        const author = document.getElementById('author').value;
        const comment = document.getElementById('comment').value;

        let fileName;
        if (srcModelIn.files.length > 0) {
            fileName = srcModelIn.files[0].name;
        } else {
            fileName = modelIn.files[0].name;
        }

        const m = fileName.match(/^(.+)_(src|tgt).mdl$/);
        if (!m) {
            stdout.innerHTML += '<p style="color: red">Upload error: model file name must match /^(.+)_tgt.mdl</p>';
            return;
        }
        const name = m[1];

        const traceIn = document.getElementById('traceFileIn');
        const reader = new FileReader();
        reader.onload = async (e) => {
            const formData = new FormData();
            formData.append('name', name);
            formData.append('energy', energy);
            formData.append('author', author);
            formData.append('comment', comment);
            console.log(e.target.result);
            formData.append('nbt-blob', new Blob([e.target.result]));
            console.log(formData);
            try {
                const response = await fetch(`${base}/traces/register`, {
                    method: 'POST',
                    body: formData,
                });
                const json = await response.json();
                if (json.status === 'success') {
                    stdout.innerHTML += '<p style="color: green">Upload succeeded!</p>';
                } else {
                    const err = json.message;
                    stdout.innerHTML += `<p style="color: red">Upload failed: ${err}</p>`;
                }
            } catch (err) {
                stdout.innerHTML += `<p style="color: red">Network connection error: ${err}`;
            }
        };
        reader.readAsArrayBuffer(traceIn.files[0]);
    });
});