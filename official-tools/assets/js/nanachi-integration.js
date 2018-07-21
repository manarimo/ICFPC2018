document.addEventListener('DOMContentLoaded', async () => {
    const base = window.location.host === '' ? 'http://localhost:8081' : 'http://nanachi.kadingel.osak.jp';
    const params = window.location.search.substr(1)
        .split('&')
        .map(s => s.split('='))
        .reduce((acc, val) => ({...acc, [val[0]]: val[1] }), {});
    console.log(params);
    if (params.model_name) {
        const res = await fetch(`${base}/models/${params.model_name}/blob`, {
            headers: {
                'Content-Type': 'application/octet-stream'
            }
        });
        const blob = new Uint8Array(await res.arrayBuffer());
        modelBData = blob;
        console.log(blob);
        document.getElementById('viewModel').click();
    }
});