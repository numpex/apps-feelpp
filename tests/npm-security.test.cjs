const assert = require('node:assert/strict');
const { mkdtempSync, mkdirSync, writeFileSync, rmSync } = require('node:fs');
const { createRequire } = require('node:module');
const { tmpdir } = require('node:os');
const { join } = require('node:path');
const { test } = require('node:test');

const globConsumers = ['@antora/content-aggregator', '@antora/ui-loader', '@antora/collector-extension'];
const braceModules = new Map();
for (const consumer of globConsumers) {
    const fromConsumer = createRequire(require.resolve(consumer));
    if (consumer !== '@antora/collector-extension') {
        braceModules.set(`${consumer} direct`, fromConsumer);
    }
    const fromGlob = createRequire(fromConsumer.resolve('fast-glob'));
    braceModules.set(`${consumer} via micromatch`, createRequire(fromGlob.resolve('micromatch')));
}

for (const [consumer, fromConsumer] of braceModules) {
    test(`${consumer}: guarded implementation and normal brace semantics`, () => {
        const metadata = fromConsumer('braces/package.json');
        assert.equal(metadata.name, '@dieub/braces-depth-guard');
        assert.equal(metadata.version, '3.0.3-pn.3');
        const braces = fromConsumer('braces');
        assert.deepEqual(braces.expand('{docs,src}/{1..3}'),
            ['docs/1', 'docs/2', 'docs/3', 'src/1', 'src/2', 'src/3']);
        assert.deepEqual(braces('a/{b,c}'), ['a/(b|c)']);
    });

    test(`${consumer}: excessive nesting cannot reach recursive walkers`, () => {
        const braces = fromConsumer('braces');
        for (const [open, close] of [['{', '}'], ['(', ')'], ['{(', ')}']]) {
            const input = open.repeat(2000) + 'x' + close.repeat(2000);
            for (const operation of [braces, braces.expand, braces.parse]) {
                // A deliberate depth rejection is required, not a native stack overflow.
                assert.throws(() => operation(input), /exceeds max depth \(100\)/);
                assert.throws(() => operation(input, { maxDepth: Infinity }), /exceeds max depth \(100\)/);
            }
        }
        const ast = { type: 'root', nodes: [] };
        let cursor = ast;
        for (let i = 0; i < 2000; ++i) {
            const child = { type: 'brace', nodes: [], parent: cursor };
            cursor.nodes.push(child);
            cursor = child;
        }
        for (const operation of [braces.compile, braces.expand, braces.stringify]) {
            assert.throws(() => operation(ast), /exceeds max depth \(100\)/);
        }
    });
}

for (const consumer of globConsumers) {
    test(`${consumer}: actual glob consumer finds nested documentation files`, () => {
        const fromConsumer = createRequire(require.resolve(consumer));
        const glob = fromConsumer('fast-glob');
        const folder = mkdtempSync(join(tmpdir(), 'apps-feelpp-glob-'));
        try {
            for (const path of ['docs/nested', 'src/nested', 'ignored']) mkdirSync(join(folder, path), { recursive: true });
            for (const path of ['docs/nested/index.adoc', 'src/nested/config.json', 'ignored/other.adoc']) {
                writeFileSync(join(folder, path), 'fixture');
            }
            assert.deepEqual(glob.sync('{docs,src}/**/*.{adoc,json}', { cwd: folder }).sort(),
                ['docs/nested/index.adoc', 'src/nested/config.json']);
        } finally {
            rmSync(folder, { recursive: true, force: true });
        }
    });
}

for (const consumer of [...globConsumers, '@antora/playbook-builder']) {
    test(`${consumer}: patched YAML retains configuration parsing`, () => {
        const fromConsumer = createRequire(require.resolve(consumer));
        const version = fromConsumer('js-yaml/package.json').version.split('.').map(Number);
        assert.ok(version[0] >= 5 || (version[0] === 4 && (version[1] > 3 ||
            (version[1] === 3 && version[2] >= 2))));
        const yaml = fromConsumer('js-yaml');
        const config = { site: { title: 'Feel++', enabled: true }, sources: ['docs', 'src'] };
        assert.deepEqual(yaml.load(yaml.dump(config), { schema: yaml.CORE_SCHEMA }), config);
    });
}
