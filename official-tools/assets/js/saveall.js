async function saveAll(event) {
    for(const f of event.target.files) {
        if (!f) {
            console.error('Cannot read ' + f);
            continue;
        }
        const reader = new FileReader();
        await new Promise((resolve) => {
            reader.onload = async function (e) {
                modelBData = new Uint8Array(e.target.result);
                document.getElementById('viewModel').click();
                setTimeout(() => {
                    vis.setRotation(Math.PI / 4, -Math.PI / 4);
                    vis.render();
                    vis.postImage('http://localhost:25252', `${f.name}-up-315deg.png`);
                    resolve();
                }, 5000);
            };
            reader.readAsArrayBuffer(f);
        });
    }
}

document.getElementById('thumbnailFiles').addEventListener('change', saveAll, false);
