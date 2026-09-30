addToLibrary({
    warshipsStorageList__proxy: "sync",
    warshipsStorageList__deps: ["$UTF8ToString", "$stringToNewUTF8"],
    warshipsStorageList: function (prefixPointer) {
        const prefix = UTF8ToString(prefixPointer);
        const names = [];

        for (let index = 0; index < localStorage.length; ++index) {
            const key = localStorage.key(index);
            if (key !== null && key.startsWith(prefix)) {
                names.push(key.slice(prefix.length));
            }
        }

        return stringToNewUTF8(names.join("\n"));
    },

    warshipsStorageRead__proxy: "sync",
    warshipsStorageRead__deps: ["$UTF8ToString", "$stringToNewUTF8"],
    warshipsStorageRead: function (keyPointer) {
        const contents = localStorage.getItem(UTF8ToString(keyPointer));

        return contents === null ? 0 : stringToNewUTF8(contents);
    },

    warshipsStorageWrite__proxy: "sync",
    warshipsStorageWrite__deps: ["$UTF8ToString"],
    warshipsStorageWrite: function (keyPointer, contentsPointer) {
        try {
            localStorage.setItem(UTF8ToString(keyPointer), UTF8ToString(contentsPointer));
            return 1;
        } catch (outOfRoom) {
            return 0;
        }
    },

    warshipsStorageContains__proxy: "sync",
    warshipsStorageContains__deps: ["$UTF8ToString"],
    warshipsStorageContains: function (keyPointer) {
        return localStorage.getItem(UTF8ToString(keyPointer)) === null ? 0 : 1;
    },

    warshipsStorageRemove__proxy: "sync",
    warshipsStorageRemove__deps: ["$UTF8ToString"],
    warshipsStorageRemove: function (keyPointer) {
        const key = UTF8ToString(keyPointer);
        if (localStorage.getItem(key) === null) {
            return 0;
        }

        localStorage.removeItem(key);
        return 1;
    },
});
