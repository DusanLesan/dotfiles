module.exports.themes = [{
	name: 'vscode',
	displayName: 'VSCode',
	theme: {
		rawCss: `
			.app > div > div {
				grid-template: "Header Header" 0px
					"Banner Banner" 0px
					"Navbar Content" 1fr
					"Statusbar Statusbar" 0px [row-end] / 0px 1fr;
			}
			.notice.error {
				border-style: unset;
				background-color: var(--color-danger);
				color: white;
			}
			.notice.success {
				border-style: unset;
				background-color: var(--color-success);
				color: black;
			}
			.notice.warning {
				border-style: unset;
				background-color: var(--color-warning);
				color: black;
			}
			.notice.info {
				border-style: unset;
				background-color: var(--color-info);
				color: black;
			}
			div[aria-label="Insomnia Tabs"] {
				height: 26px !important;
			}
			div[class*="box-content"] {
				height: 26px !important;
			}
		`,
		background: {
			default: '#181818',
			success: '#50fa7b',
			notice: '#f1fa8c',
			warning: '#ffb86c',
			danger: '#ff5555',
			surprise: '#35A0F3',
			info: '#8be9fd'
		},
		foreground: {
			default: '#f8f8f2',
			success: '#282a36',
			notice: '#282a36',
			warning: '#282a36',
			danger: '#282a36',
			surprise: '#282a36',
			info: '#282a36'
		},
		highlight: {
			md: '#1E1E1E',
		},
		styles: {
			sidebar: {
				background: {
					default: '#181818',
				},
				highlight: {
					default: '#cccccc',
					xxs: 'rgba(98, 98, 98, 0.05)',
					xs: 'rgba(98, 98, 98, 0.1)',
					sm: 'rgba(98, 98, 98, 0.2)',
					md: 'rgba(98, 98, 98, 0.4)',
					lg: 'rgba(98, 98, 98, 0.6)',
					xl: 'rgba(98, 98, 98, 0.8)'
				},
				foreground: {
					default: '#cccccc',
				},
			},
			pane: {
				background: {
					default: '#181818',
					success: '#9CDCFE',
					notice: '#CE9178',
					warning: '#f1fa8c',
					danger: '#ff5555',
					surprise: '#35A0F3',
					info: '#B5CEA8'
				},
				highlight: {
					default: '#8F8F8F',
					xs: '#37373D',
					md: '#1E1E1E',
					lg: '#A9A9A9',
					xl: '#6D6D6D'
				},
			},
			transparentOverlay: {
				background: {
					default: 'rgba(40, 40, 40, 0.5)',
				}
			},
			dialog: {
				background: {
					default: '#1E1E1E',
				}
			},
			link: {
				background: {
					surprise: '#CE9178',
				},
			},
			sidebarList: {
				background: {
					success: '#B5CEA8',
					surprise: '#35A0F3',
				},
				foreground: {
					default: '#cccccc',
				},
				highlight: {
					default: '#cccccc',
					xs: '#37373D',
				},
			},
		}
	}
}]
