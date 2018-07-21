function waitBeforeRun(f, wait) {
    return new Promise((resolve) => {
        setTimeout(() => {
            f();
            resolve();
        }, wait);
    });
}

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
                await waitBeforeRun(() => {
                    vis.render();
                    vis.postImage('http://localhost:25252', `${f.name}.png`);
                }, 2000);
                await waitBeforeRun(() => {
                    vis.setRotation(Math.PI / 4, Math.PI / 4);
                    vis.render();
                    vis.postImage('http://localhost:25252', `${f.name}-up-45deg.png`);
                }, 2000);
                await waitBeforeRun(() => {
                    vis.setRotation(Math.PI / 4, -Math.PI / 4);
                    vis.render();
                    vis.postImage('http://localhost:25252', `${f.name}-up-315deg.png`);
                }, 2000);
                resolve();
            };
            reader.readAsArrayBuffer(f);
        });
    }
}

document.getElementById('thumbnailFiles').addEventListener('change', saveAll, false);
